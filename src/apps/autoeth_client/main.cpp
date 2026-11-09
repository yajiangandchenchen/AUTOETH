// AUTOETH autoeth-client: demonstration client CLI (S8).
// R10.10: exit codes 0 success, 2 argument/config, 3 NoService/Timeout,
// 4 RemoteApplication, 5 protocol/E2E, 6 IO/Resource, 130 interrupt.
#include <csignal>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include "autoeth/build_info.hpp"
#include "autoeth/runtime.hpp"

namespace app_autoeth_client_detail {

volatile std::sig_atomic_t g_interrupted = 0;

void HandleSignal(int signal_number) {
  (void)signal_number;
  g_interrupted = 1;
}

struct Options {
  std::string command;
  std::string config;
  std::string service;
  std::string field;
  std::string value_path;
  std::string transport;
  std::uint64_t timeout_ns;
  std::uint64_t offset;
  std::uint64_t length;
  std::uint32_t group;
  std::uint32_t event_count;
  std::uint64_t duration_ns;

  Options() noexcept
      : service("vehicle"), timeout_ns(1000000000u), offset(0u), length(32u), group(0u),
        event_count(0u), duration_ns(0u) {}
};

void PrintUsage() {
  std::fprintf(stderr,
               "usage: autoeth-client <command> --config <file> [--service vehicle] "
               "[--timeout-ns <ns>]\n"
               "commands: discover read-snapshot read-bulk apply-calibration record-command "
               "field-get field-set subscribe\n");
}

int ExitCodeForStatus(const autoeth::Status& status) {
  switch (static_cast<autoeth::Errc>(status.typed_code)) {
    case autoeth::Ok:
      return 0;
    case autoeth::InvalidConfig:
    case autoeth::NullRange:
      return 2;
    case autoeth::NoService:
    case autoeth::Timeout:
    case autoeth::NotReady:
      return 3;
    case autoeth::RemoteApplication:
      return 4;
    case autoeth::RemoteProtocol:
    case autoeth::ProtocolVersion:
    case autoeth::PayloadMalformed:
      return 5;
    case autoeth::Io:
    case autoeth::Resource:
    default:
      return 6;
  }
}

bool ParseArgs(int argc, char** argv, Options& options) {
  if (argc < 2) {
    PrintUsage();
    return false;
  }
  options.command = argv[1];
  for (int index = 2; index < argc; ++index) {
    const std::string argument = argv[index];
    const bool has_value = (index + 1) < argc;
    if (argument == "--config" && has_value) {
      options.config = argv[++index];
    } else if (argument == "--service" && has_value) {
      options.service = argv[++index];
    } else if (argument == "--field" && has_value) {
      options.field = argv[++index];
    } else if (argument == "--value" && has_value) {
      options.value_path = argv[++index];
    } else if (argument == "--transport" && has_value) {
      options.transport = argv[++index];
    } else if (argument == "--timeout-ns" && has_value) {
      options.timeout_ns = std::strtoull(argv[++index], nullptr, 10);
    } else if (argument == "--offset" && has_value) {
      options.offset = std::strtoull(argv[++index], nullptr, 10);
    } else if (argument == "--length" && has_value) {
      options.length = std::strtoull(argv[++index], nullptr, 10);
    } else if (argument == "--group" && has_value) {
      options.group = static_cast<std::uint32_t>(std::strtoul(argv[++index], nullptr, 10));
    } else if (argument == "--event-count" && has_value) {
      options.event_count = static_cast<std::uint32_t>(std::strtoul(argv[++index], nullptr, 10));
    } else if (argument == "--duration-ns" && has_value) {
      options.duration_ns = std::strtoull(argv[++index], nullptr, 10);
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

}  // namespace app_autoeth_client_detail

using app_autoeth_client_detail::Options;
using app_autoeth_client_detail::g_interrupted;

using app_autoeth_client_detail::HandleSignal;
using app_autoeth_client_detail::PrintUsage;
using app_autoeth_client_detail::ExitCodeForStatus;
using app_autoeth_client_detail::ParseArgs;
using app_autoeth_client_detail::ReadWholeFile;

int main(int argc, char** argv) {
  Options options;
  if (!ParseArgs(argc, argv, options)) {
    return 2;
  }
  std::string config_text;
  if (!ReadWholeFile(options.config, config_text)) {
    std::fprintf(stderr, "autoeth-client: cannot read config %s\n", options.config.c_str());
    return 2;
  }
  autoeth::RuntimeConfig config;
  autoeth::ConfigDiagnostic diagnostic;
  const autoeth::ByteView view(reinterpret_cast<const std::uint8_t*>(config_text.data()),
                               config_text.size());
  const autoeth::Status load_status = autoeth::load_runtime_config(view, ".", config, &diagnostic);
  if (!load_status.IsOk()) {
    std::fprintf(stderr, "autoeth-client: config rejected at %s\n", diagnostic.json_path.c_str());
    return 2;
  }
  autoeth::RuntimeHandle runtime;
  const autoeth::Status create_status = autoeth::create_runtime(config, runtime);
  if (!create_status.IsOk() || runtime == nullptr) {
    return ExitCodeForStatus(create_status);
  }
  std::signal(SIGINT, HandleSignal);
  std::signal(SIGTERM, HandleSignal);
  if (g_interrupted != 0) {
    return 130;
  }
  const autoeth::BuildInfo info = autoeth::build_info();
  std::printf("autoeth-client %s command=%s service=%s timeout-ns=%llu\n", info.project_version,
              options.command.c_str(), options.service.c_str(),
              static_cast<unsigned long long>(options.timeout_ns));
  const autoeth::Status start_status = runtime->start();
  if (!start_status.IsOk()) {
    return ExitCodeForStatus(start_status);
  }
  if (options.command == "discover" || options.command == "read-snapshot" ||
      options.command == "read-bulk" || options.command == "apply-calibration" ||
      options.command == "record-command" || options.command == "field-get" ||
      options.command == "field-set" || options.command == "subscribe") {
    std::printf("autoeth-client: command accepted; transport stage not yet attached, "
                "no server execution is implied\n");
    return 0;
  }
  std::fprintf(stderr, "autoeth-client: unknown command %s\n", options.command.c_str());
  return 2;
}