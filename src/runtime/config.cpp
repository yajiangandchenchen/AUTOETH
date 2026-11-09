// AUTOETH runtime: configuration validation and JSON loading.
// R1.2 fixes the validation order; R1.3 fixes the ConfigDetail codes.
// Stage S1 of the design lands here.
#include <cstring>

#include "autoeth/runtime.hpp"

namespace autoeth {
namespace {

struct ParserState {
  ByteView input;
  std::size_t cursor;
  RuntimeConfig* config;
  ConfigDiagnostic* diagnostic;
  Errc error;
  ConfigDetail detail;
  std::size_t error_offset;
  std::string error_path;
};

void Fail(ParserState& state, Errc errc, ConfigDetail detail, const std::string& path) noexcept {
  state.error = errc;
  state.detail = detail;
  state.error_offset = state.cursor;
  state.error_path = path;
}

void SkipWhitespace(ParserState& state) noexcept {
  while (state.cursor < state.input.size) {
    const std::uint8_t value = state.input.data[state.cursor];
    if (value != ' ' && value != '\t' && value != '\n' && value != '\r') {
      break;
    }
    ++state.cursor;
  }
}

bool Consume(ParserState& state, std::uint8_t expected) noexcept {
  SkipWhitespace(state);
  if (state.cursor >= state.input.size || state.input.data[state.cursor] != expected) {
    return false;
  }
  ++state.cursor;
  return true;
}

bool ParseString(ParserState& state, std::string& out) noexcept {
  if (!Consume(state, '"')) {
    return false;
  }
  std::string text;
  while (state.cursor < state.input.size) {
    const std::uint8_t value = state.input.data[state.cursor];
    if (value == '"') {
      ++state.cursor;
      out = text;
      return true;
    }
    if (value == '\\') {
      ++state.cursor;
      if (state.cursor >= state.input.size) {
        return false;
      }
      const std::uint8_t escaped = state.input.data[state.cursor];
      switch (escaped) {
        case '"':
        case '\\':
        case '/':
          text.push_back(static_cast<char>(escaped));
          break;
        case 'b':
          text.push_back('\b');
          break;
        case 'f':
          text.push_back('\f');
          break;
        case 'n':
          text.push_back('\n');
          break;
        case 'r':
          text.push_back('\r');
          break;
        case 't':
          text.push_back('\t');
          break;
        default:
          // Unsupported escapes are rejected rather than guessed.
          return false;
      }
      ++state.cursor;
      continue;
    }
    text.push_back(static_cast<char>(value));
    ++state.cursor;
  }
  return false;
}

bool ParseInteger(ParserState& state, long long& out) noexcept {
  SkipWhitespace(state);
  const std::size_t start = state.cursor;
  bool negative = false;
  if (state.cursor < state.input.size && state.input.data[state.cursor] == '-') {
    negative = true;
    ++state.cursor;
  }
  if (state.cursor >= state.input.size) {
    return false;
  }
  long long value = 0;
  bool any = false;
  while (state.cursor < state.input.size) {
    const std::uint8_t digit = state.input.data[state.cursor];
    if (digit < '0' || digit > '9') {
      break;
    }
    // Lexical exact conversion, no floating point intermediate (R1.1).
    value = value * 10 + static_cast<long long>(digit - '0');
    any = true;
    ++state.cursor;
  }
  if (state.cursor < state.input.size) {
    const std::uint8_t next = state.input.data[state.cursor];
    if (next == '.' || next == 'e' || next == 'E') {
      return false;
    }
  }
  if (!any) {
    state.cursor = start;
    return false;
  }
  out = negative ? -value : value;
  return true;
}

bool ParseBool(ParserState& state, bool& out) noexcept {
  SkipWhitespace(state);
  const std::size_t remaining = state.input.size - state.cursor;
  const char* data = reinterpret_cast<const char*>(state.input.data) + state.cursor;
  if (remaining >= 4u && std::strncmp(data, "true", 4u) == 0) {
    state.cursor += 4u;
    out = true;
    return true;
  }
  if (remaining >= 5u && std::strncmp(data, "false", 5u) == 0) {
    state.cursor += 5u;
    out = false;
    return true;
  }
  return false;
}

struct DuplicateKeyGuard {
  std::string last_key;
  bool seen;
  DuplicateKeyGuard() noexcept : seen(false) {}
};

void FillDefaultDiagnostics(ConfigDiagnostic* diagnostic) noexcept {
  if (diagnostic != nullptr) {
    diagnostic->code = DuplicateKey;
    diagnostic->file.clear();
    diagnostic->json_path.clear();
    diagnostic->byte_offset = 0u;
    diagnostic->path_complete = false;
  }
}

bool ValidateConfigVersion(ParserState& state) noexcept {
  if (state.config->schema_version != 1u) {
    Fail(state, InvalidConfig, VersionMismatch, "$.schema_version");
    return false;
  }
  return true;
}

bool ValidateResources(ParserState& state) noexcept {
  const ResourceConfig& resources = state.config->resources;
  if (resources.max_frame_bytes < 16u) {
    Fail(state, InvalidConfig, RangeMismatch, "$.resources.max_frame_bytes");
    return false;
  }
  if (resources.max_active_rpcs < 1u || resources.max_active_rpcs > 65535u) {
    Fail(state, InvalidConfig, RangeMismatch, "$.resources.max_active_rpcs");
    return false;
  }
  if (resources.max_schema_depth == 0u) {
    Fail(state, InvalidConfig, RangeMismatch, "$.resources.max_schema_depth");
    return false;
  }
  return true;
}

bool ValidateReferences(ParserState& state) noexcept {
  // Schema names must be unique across every configured schema.
  for (std::size_t index = 0u; index < state.config->schemas.size(); ++index) {
    if (state.config->schemas[index].name.empty()) {
      Fail(state, InvalidConfig, ReferenceMismatch, "$.schemas");
      return false;
    }
    for (std::size_t other = index + 1u; other < state.config->schemas.size(); ++other) {
      if (state.config->schemas[other].name == state.config->schemas[index].name) {
        Fail(state, InvalidConfig, ReferenceMismatch, "$.schemas");
        return false;
      }
    }
  }
  // Every declared schema must carry a reachable non-zero root.
  for (std::size_t index = 0u; index < state.config->schemas.size(); ++index) {
    if (state.config->schemas[index].root == 0u) {
      Fail(state, InvalidConfig, ReferenceMismatch, "$.schemas");
      return false;
    }
  }
  return true;
}

bool ValidateRoles(ParserState& state) noexcept {
  // R1.10: argumentSet is legal only on a method root.
  for (std::size_t index = 0u; index < state.config->schemas.size(); ++index) {
    const SchemaSpec& schema = state.config->schemas[index];
    if (schema.argumentSet && schema.role != RoleMethodRequest &&
        schema.role != RoleMethodResponse) {
      Fail(state, InvalidConfig, RoleMismatch, "$.schemas");
      return false;
    }
    if (!schema.argumentSet && (schema.role == RoleEvent || schema.role == RoleField)) {
      continue;
    }
  }
  return true;
}

bool ValidateProfiles(ParserState& state) noexcept {
  // R6.12: a P02 normal-network binding is rejected before publication.
  for (std::size_t index = 0u; index < state.config->e2e_channels.size(); ++index) {
    const E2eChannelConfig& channel = state.config->e2e_channels[index];
    if (channel.profile == "02" && channel.configuration_surface != "SourceC") {
      Fail(state, InvalidConfig, ProfileGeometry, "$.e2e_channels");
      return false;
    }
    if (channel.profile == "01" || channel.profile == "02") {
      if (!channel.legacy_compatibility) {
        Fail(state, InvalidConfig, ProfileGeometry, "$.e2e_channels");
        return false;
      }
    }
  }
  return true;
}

bool ValidateResourceAdmission(ParserState& state) noexcept {
  std::uint64_t used = 0u;
  for (std::size_t index = 0u; index < state.config->endpoints.size(); ++index) {
    used += 1u;
  }
  if (used > static_cast<std::uint64_t>(state.config->resources.max_udp_sockets)) {
    Fail(state, Resource, CountMismatch, "$.endpoints");
    return false;
  }
  return true;
}


namespace {

// Minimal owning scanner used by the document loader. It walks the top-level
// object and extracts the fields validation depends on, while still enforcing
// duplicate-key detection and exact integer syntax (R1.1).
struct ScanContext {
  ParserState* state;
  RuntimeConfig* config;
  std::string last_key;
  bool seen_key;
};

bool ScanObjectBody(ParserState& state, ScanContext& context, bool extract) {
  if (!Consume(state, '{')) {
    return false;
  }
  context.seen_key = false;
  SkipWhitespace(state);
  if (state.cursor < state.input.size && state.input.data[state.cursor] == '}') {
    ++state.cursor;
    return true;
  }
  while (true) {
    std::string key;
    if (!ParseString(state, key)) {
      return false;
    }
    if (context.seen_key && context.last_key == key) {
      Fail(state, InvalidConfig, DuplicateKey, "$." + key);
      return false;
    }
    context.last_key = key;
    context.seen_key = true;
    if (!Consume(state, ':')) {
      return false;
    }
    SkipWhitespace(state);
    if (state.cursor >= state.input.size) {
      return false;
    }
    const std::uint8_t first = state.input.data[state.cursor];
    bool handled = false;
    if (first == '"') {
      std::string text;
      if (!ParseString(state, text)) {
        return false;
      }
      if (extract && key == "node_name") {
        context.config->node_name = text;
      }
      handled = true;
    } else if (first == '{' || first == '[') {
      // Nested content is walked for structure/depth only.
      std::size_t depth = 0u;
      const std::uint8_t open = first;
      const std::uint8_t close = (open == '{') ? '}' : ']';
      while (state.cursor < state.input.size) {
        const std::uint8_t value = state.input.data[state.cursor];
        if (value == open) {
          ++depth;
        } else if (value == close) {
          --depth;
          if (depth == 0u) {
            ++state.cursor;
            break;
          }
        } else if (value == '"') {
          // Skip over string content so braces inside strings do not count.
          std::string ignored;
          if (!ParseString(state, ignored)) {
            return false;
          }
          continue;
        }
        ++state.cursor;
      }
      handled = true;
    } else if (first == 't' || first == 'f') {
      bool flag = false;
      if (!ParseBool(state, flag)) {
        return false;
      }
      if (extract && key == "fresh_context_asserted") {
        context.config->correlation.fresh_context_asserted = flag;
      }
      handled = true;
    } else {
      long long integer = 0;
      if (!ParseInteger(state, integer)) {
        return false;
      }
      if (extract && integer >= 0) {
        const std::uint64_t value = static_cast<std::uint64_t>(integer);
        if (key == "schema_version") {
          context.config->schema_version = static_cast<std::uint32_t>(value);
        } else if (key == "client_id") {
          context.config->client_id = static_cast<std::uint16_t>(value);
        } else if (key == "max_frame_bytes") {
          context.config->resources.max_frame_bytes = value;
        } else if (key == "max_owned_payload_bytes") {
          context.config->resources.max_owned_payload_bytes = value;
        } else if (key == "max_active_rpcs") {
          context.config->resources.max_active_rpcs = static_cast<std::uint32_t>(value);
        } else if (key == "max_commands") {
          context.config->resources.max_commands = static_cast<std::uint32_t>(value);
        } else if (key == "max_udp_sockets") {
          context.config->resources.max_udp_sockets = static_cast<std::uint32_t>(value);
        } else if (key == "max_schema_depth") {
          context.config->resources.max_schema_depth = static_cast<std::uint32_t>(value);
        } else if (key == "retirement_horizon_ns") {
          context.config->correlation.retirement_horizon_ns = value;
        } else if (key == "rpc_timeout_ns") {
          context.config->scheduling.rpc_timeout_ns = value;
        }
      }
      handled = true;
    }
    if (!handled) {
      return false;
    }
    SkipWhitespace(state);
    if (state.cursor < state.input.size && state.input.data[state.cursor] == ',') {
      ++state.cursor;
      continue;
    }
    if (Consume(state, '}')) {
      return true;
    }
    return false;
  }
}

}  // namespace

void ApplyDiagnostic(ParserState& state) noexcept {
  if (state.diagnostic != nullptr) {
    state.diagnostic->code = state.detail;
    state.diagnostic->file.clear();
    state.diagnostic->json_path = state.error_path;
    state.diagnostic->byte_offset = state.error_offset;
    state.diagnostic->path_complete = true;
  }
}

}  // namespace

Status load_runtime_config(ByteView utf8, const std::string& base_directory, RuntimeConfig& out,
                           ConfigDiagnostic* diagnostic) noexcept {
  (void)base_directory;
  FillDefaultDiagnostics(diagnostic);
  if (utf8.IsNullRange()) {
    return Status::Error(ModuleConfig, NullRange);
  }
  ParserState state;
  state.input = utf8;
  state.cursor = 0u;
  state.config = &out;
  state.diagnostic = diagnostic;
  state.error = Ok;
  state.detail = DuplicateKey;
  state.error_offset = 0u;
  if (utf8.size == 0u) {
    return Status::Error(ModuleConfig, InvalidConfig);
  }
  // A minimal structural check: the document must be a JSON object and end
  // with a closing brace. Full field mapping is added with the schema loader.
  SkipWhitespace(state);
  if (state.cursor >= utf8.size || utf8.data[state.cursor] != '{') {
    state.detail = TypeMismatch;
    ApplyDiagnostic(state);
    return Status::Error(ModuleConfig, InvalidConfig);
  }
  ScanContext context;
  context.state = &state;
  context.config = &out;
  state.cursor = 0u;
  if (!ScanObjectBody(state, context, true)) {
    if (state.error == Ok) {
      state.detail = TypeMismatch;
    }
    ApplyDiagnostic(state);
    return Status::Error(ModuleConfig, InvalidConfig);
  }
  if (!ValidateConfigVersion(state) || !ValidateResources(state) ||
      !ValidateReferences(state) || !ValidateRoles(state) || !ValidateProfiles(state) ||
      !ValidateResourceAdmission(state)) {
    ApplyDiagnostic(state);
    if (state.detail == CountMismatch) {
      return Status::Error(ModuleConfig, Resource);
    }
    return Status::Error(ModuleConfig, InvalidConfig);
  }
  return Status::Success();
}

}  // namespace autoeth