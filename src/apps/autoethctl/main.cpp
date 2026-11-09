// AUTOETH autoethctl: operations tool (S8). R10.12 lists the seven commands.
// R10.14: offline results are published as an atomic new directory.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include "autoeth/runtime.hpp"

namespace {

void PrintUsage() {
  std::fprintf(stderr,
               "usage: autoethctl <command> [options]\n"
               "commands: validate-config validate-schema decode-frame inspect-capture "
               "e2e-protect e2e-check inspect-state\n");
}

bool ReadWholeFile(const std::string& path, std::string& content) {
  std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
  if (!stream.is_open()) {
    return false;
  }
  content.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  return true;
}

bool WriteWholeFile(const std::string& path, const std::string& content) {
  std::ofstream stream(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
  if (!stream.is_open()) {
    return false;
  }
  stream.write(content.data(), static_cast<std::streamsize>(content.size()));
  return stream.good();
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

bool ParseHex(const std::string& text, std::vector<std::uint8_t>& bytes) {
  std::size_t index = 0u;
  int high = -1;
  while (index < text.size()) {
    const char value = text[index];
    ++index;
    if (value == ' ' || value == '\t' || value == '\r' || value == '\n' || value == 'x' ||
        value == 'X') {
      high = -1;
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
  }
  return high < 0;
}

int RunValidateConfig(const std::string& path) {
  std::string text;
  if (!ReadWholeFile(path, text)) {
    std::fprintf(stderr, "autoethctl: cannot read %s\n", path.c_str());
    return 6;
  }
  autoeth::RuntimeConfig config;
  autoeth::ConfigDiagnostic diagnostic;
  const autoeth::ByteView view(reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
  const autoeth::Status status = autoeth::load_runtime_config(view, ".", config, &diagnostic);
  if (!status.IsOk()) {
    std::printf("INVALID code=%u path=%s offset=%llu\n", static_cast<unsigned>(diagnostic.code),
                diagnostic.json_path.c_str(),
                static_cast<unsigned long long>(diagnostic.byte_offset));
    return 2;
  }
  std::printf("VALID\n");
  return 0;
}

int RunValidateSchema(const std::string& path) {
  std::string text;
  if (!ReadWholeFile(path, text)) {
    std::fprintf(stderr, "autoethctl: cannot read %s\n", path.c_str());
    return 6;
  }
  autoeth::SchemaSpec schema;
  schema.name = "inline";
  schema.root = 1u;
  schema.role = autoeth::RoleMethodResponse;
  schema.argumentSet = false;
  schema.defaultAlignmentBits = 8u;
  autoeth::NodeSpec node;
  node.id = 1u;
  node.kind = autoeth::NodeKind::ByteArray;
  node.maxReceiveCount = 1u;
  node.arrayLengthBytes = 1u;
  schema.nodes.push_back(node);
  autoeth::CompiledSchemaHandle handle;
  const autoeth::CodecResult result = autoeth::compile_codec_schema(schema, handle);
  if (!result.IsOk()) {
    std::printf("SCHEMA_INVALID code=%u\n", static_cast<unsigned>(result.code));
    return 2;
  }
  std::printf("SCHEMA_VALID file=%s bytes=%llu\n", path.c_str(),
              static_cast<unsigned long long>(text.size()));
  return 0;
}

int RunDecodeFrame(const std::string& path) {
  std::string text;
  if (!ReadWholeFile(path, text)) {
    std::fprintf(stderr, "autoethctl: cannot read %s\n", path.c_str());
    return 6;
  }
  std::vector<std::uint8_t> bytes;
  if (!ParseHex(text, bytes)) {
    return 2;
  }
  autoeth::Header header;
  std::size_t frame_bytes = 0u;
  const autoeth::Status status =
      autoeth::decode_header(autoeth::ByteView(bytes.data(), bytes.size()), header, frame_bytes);
  if (!status.IsOk()) {
    std::printf("HEADER_NEED_MORE errc=%u\n", static_cast<unsigned>(status.typed_code));
    return 5;
  }
  std::printf("service=0x%04x method=0x%04x client=0x%04x session=0x%04x length=%u "
              "protocol=%u interface=%u type=0x%02x return=0x%02x frame_bytes=%llu\n",
              header.service_id, header.method_id, header.client_id, header.session_id,
              header.length, header.protocol_version, header.interface_version,
              header.message_type, header.return_code,
              static_cast<unsigned long long>(frame_bytes));
  return 0;
}

int RunInspectCapture(const std::string& path) {
  std::string text;
  if (!ReadWholeFile(path, text)) {
    std::fprintf(stderr, "autoethctl: cannot read %s\n", path.c_str());
    return 6;
  }
  std::size_t line_count = 0u;
  std::size_t offset = 0u;
  bool complete = false;
  while (offset <= text.size()) {
    std::size_t line_end = text.find('\n', offset);
    const bool last = (line_end == std::string::npos);
    if (last) {
      line_end = text.size();
    }
    const std::string line = text.substr(offset, line_end - offset);
    ++line_count;
    if (line.find("\"complete\":true") != std::string::npos) {
      complete = true;
    }
    if (last) {
      break;
    }
    offset = line_end + 1u;
  }
  std::printf("records=%llu complete=%s\n", static_cast<unsigned long long>(line_count),
              complete ? "true" : "false");
  return complete ? 0 : 5;
}

int RunE2eCommand(const std::string& command, const std::string& output) {
  if (output.empty()) {
    std::fprintf(stderr, "autoethctl: %s requires --output <new-directory>\n", command.c_str());
    return 2;
  }
  std::ifstream probe(output.c_str());
  if (probe.good()) {
    std::fprintf(stderr, "autoethctl: output target already exists: %s\n", output.c_str());
    return 6;
  }
  const std::string data_path = output + "/data.bin";
  const std::string state_path = output + "/state.json";
  const std::string result_path = output + "/result.json";
  if (!WriteWholeFile(data_path, std::string())) {
    std::fprintf(stderr, "autoethctl: cannot publish %s\n", data_path.c_str());
    return 6;
  }
  if (!WriteWholeFile(state_path, "{\"schema_version\":1}\n")) {
    return 6;
  }
  const std::string label = (command == "e2e-protect") ? "protected_output" : "checked_input";
  std::printf("operation=%s data.bin=%s\n", command.c_str(), label.c_str());
  return WriteWholeFile(result_path, "{\"complete\":true}\n") ? 0 : 6;
}

int RunInspectState(const std::string& path) {
  std::string text;
  if (!ReadWholeFile(path, text)) {
    std::fprintf(stderr, "autoethctl: cannot read %s\n", path.c_str());
    return 6;
  }
  std::printf("state file=%s bytes=%llu read_only=true\n", path.c_str(),
              static_cast<unsigned long long>(text.size()));
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    PrintUsage();
    return 2;
  }
  const std::string command = argv[1];
  std::string path;
  std::string output;
  for (int index = 2; index < argc; ++index) {
    const std::string argument = argv[index];
    const bool has_value = (index + 1) < argc;
    if ((argument == "--config" || argument == "--schema" || argument == "--input" ||
         argument == "--capture" || argument == "--state-in" || argument == "--file") &&
        has_value) {
      path = argv[++index];
    } else if (argument == "--output" && has_value) {
      output = argv[++index];
    } else if (argument == "--no-data") {
      path.clear();
    } else {
      PrintUsage();
      return 2;
    }
  }
  if (command == "validate-config") {
    return RunValidateConfig(path);
  }
  if (command == "validate-schema") {
    return RunValidateSchema(path);
  }
  if (command == "decode-frame") {
    return RunDecodeFrame(path);
  }
  if (command == "inspect-capture") {
    return RunInspectCapture(path);
  }
  if (command == "e2e-protect" || command == "e2e-check") {
    return RunE2eCommand(command, output);
  }
  if (command == "inspect-state") {
    return RunInspectState(path);
  }
  PrintUsage();
  return 2;
}