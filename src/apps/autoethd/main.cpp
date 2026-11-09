// AUTOETH autoethd: VehicleService provider daemon (S8, milestone M1 scope).
#include <csignal>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "autoeth/build_info.hpp"
#include "autoeth/runtime.hpp"

namespace app_autoethd_detail {

volatile std::sig_atomic_t g_stop_requested = 0;

void HandleSignal(int signal_number) {
  (void)signal_number;
  g_stop_requested = 1;
}

struct Options {
  std::string config;
  std::string dataset;
  std::string dataset_format;
  std::uint64_t event_period_ns;

  Options() noexcept : dataset_format("binary"), event_period_ns(100000000u) {}
};

void PrintUsage() {
  std::fprintf(stderr,
               "usage: autoethd --config <file> [--dataset <file>] "
               "[--dataset-format binary|hex] [--event-period-ns <ns>]\n");
}

bool ParseArgs(int argc, char** argv, Options& options) {
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    const bool has_value = (index + 1) < argc;
    if (argument == "--config" && has_value) {
      options.config = argv[++index];
    } else if (argument == "--dataset" && has_value) {
      options.dataset = argv[++index];
    } else if (argument == "--dataset-format" && has_value) {
      options.dataset_format = argv[++index];
    } else if (argument == "--event-period-ns" && has_value) {
      options.event_period_ns = std::strtoull(argv[++index], nullptr, 10);
    } else if (argument == "--help") {
      PrintUsage();
      return false;
    } else {
      PrintUsage();
      return false;
    }
  }
  if (options.config.empty()) {
    PrintUsage();
    return false;
  }
  return true;
}

bool ReadWholeFile(const std::string& path, std::string& content) {
  std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
  if (!stream.is_open()) {
    return false;
  }
  content.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  return true;
}

int HexNibble(char value) {
  if (value >= '0' && value <= '9') {
    return value - '0';
  }
  if (value >= 'a' && value <= 'f') {
    return value - 'a' + 10;
  }
  if (value >= 'A' && value <= 'F') {
    return value - 'A' + 10;
  }
  return -1;
}

bool ParseHexDataset(const std::string& text, std::vector<std::uint8_t>& bytes) {
  std::size_t index = 0u;
  int high = -1;
  while (index < text.size()) {
    const char value = text[index];
    if (value == ' ' || value == '\t' || value == '\r' || value == '\n') {
      ++index;
      continue;
    }
    if (value == 'x' || value == 'X') {
      high = -1;
      ++index;
      continue;
    }
    const int nibble = HexNibble(value);
    if (nibble < 0) {
      return false;
    }
    if (high < 0) {
      high = nibble;
    } else {
      bytes.push_back(static_cast<std::uint8_t>((high << 4) | nibble));
      high = -1;
    }
    ++index;
  }
  return high < 0;
}

std::uint32_t Fnv1a(const std::vector<std::uint8_t>& bytes) {
  std::uint32_t hash = 2166136261u;
  for (std::size_t index = 0u; index < bytes.size(); ++index) {
    hash = (hash ^ bytes[index]) * 16777619u;
  }
  return hash;
}

}  // namespace app_autoethd_detail

using app_autoethd_detail::Options;
using app_autoethd_detail::g_stop_requested;

using app_autoethd_detail::HandleSignal;
using app_autoethd_detail::PrintUsage;
using app_autoethd_detail::ParseArgs;
using app_autoethd_detail::ReadWholeFile;
using app_autoethd_detail::HexNibble;
using app_autoethd_detail::ParseHexDataset;
using app_autoethd_detail::Fnv1a;

int main(int argc, char** argv) {
  Options options;
  if (!ParseArgs(argc, argv, options)) {
    return 2;
  }
  std::string config_text;
  if (!ReadWholeFile(options.config, config_text)) {
    std::fprintf(stderr, "autoethd: cannot read config %s\n", options.config.c_str());
    return 2;
  }
  autoeth::RuntimeConfig config;
  autoeth::ConfigDiagnostic diagnostic;
  const autoeth::ByteView view(reinterpret_cast<const std::uint8_t*>(config_text.data()),
                               config_text.size());
  const autoeth::Status load_status = autoeth::load_runtime_config(view, ".", config, &diagnostic);
  if (!load_status.IsOk()) {
    std::fprintf(stderr, "autoethd: config rejected at %s (%llu)\n",
                 diagnostic.json_path.c_str(),
                 static_cast<unsigned long long>(diagnostic.byte_offset));
    return 2;
  }
  std::vector<std::uint8_t> dataset;
  if (!options.dataset.empty()) {
    std::string dataset_text;
    if (!ReadWholeFile(options.dataset, dataset_text)) {
      std::fprintf(stderr, "autoethd: cannot read dataset %s\n", options.dataset.c_str());
      return 2;
    }
    if (options.dataset_format == "hex") {
      if (!ParseHexDataset(dataset_text, dataset)) {
        std::fprintf(stderr, "autoethd: malformed hex dataset\n");
        return 2;
      }
    } else {
      dataset.assign(dataset_text.begin(), dataset_text.end());
    }
    std::printf("autoethd: dataset bytes=%llu hash=%08x\n",
                static_cast<unsigned long long>(dataset.size()), Fnv1a(dataset));
  }
  autoeth::RuntimeHandle runtime;
  if (!autoeth::create_runtime(config, runtime).IsOk() || runtime == nullptr) {
    std::fprintf(stderr, "autoethd: cannot create runtime\n");
    return 6;
  }
  std::signal(SIGINT, HandleSignal);
  std::signal(SIGTERM, HandleSignal);
  const autoeth::BuildInfo info = autoeth::build_info();
  std::printf("autoethd %s (SOME/IP %s) event-period-ns=%llu\n", info.project_version,
              info.standard_release, static_cast<unsigned long long>(options.event_period_ns));
  const autoeth::Status start_status = runtime->start();
  if (!start_status.IsOk()) {
    std::fprintf(stderr, "autoethd: start failed\n");
    return 6;
  }
  std::printf("autoethd: ready (local admission only, no peer execution implied)\n");
  while (g_stop_requested == 0) {
    // Placeholder wait: real transport ingress runs on the owner loop added in
    // the transport stage; this keeps SIGINT/SIGTERM handling real today.
    struct timespec pause_time;
    pause_time.tv_sec = 0;
    pause_time.tv_nsec = 50000000;
    nanosleep(&pause_time, nullptr);
  }
  (void)runtime->request_stop(autoeth::DurationNs(0u));
  (void)runtime->stop(autoeth::DurationNs(0u));
  std::printf("autoethd: stopped\n");
  return 0;
}