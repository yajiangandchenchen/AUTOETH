// AUTOETH core: wire framing and message validation. R3.3 order:
// protocol, service, interface, method, type, ReturnCode, correlation, payload.
#include "autoeth/wire.hpp"

namespace autoeth {
namespace {

std::uint16_t ReadU16(ByteView input, std::size_t offset) noexcept {
  const std::uint32_t high = input.data[offset];
  const std::uint32_t low = input.data[offset + 1u];
  return static_cast<std::uint16_t>((high << 8u) | low);
}

std::uint32_t ReadU32(ByteView input, std::size_t offset) noexcept {
  const std::uint32_t b0 = input.data[offset];
  const std::uint32_t b1 = input.data[offset + 1u];
  const std::uint32_t b2 = input.data[offset + 2u];
  const std::uint32_t b3 = input.data[offset + 3u];
  return (b0 << 24u) | (b1 << 16u) | (b2 << 8u) | b3;
}

void WriteU16(std::uint8_t* output, std::size_t offset, std::uint16_t value) noexcept {
  output[offset] = static_cast<std::uint8_t>((value >> 8u) & 0xFFu);
  output[offset + 1u] = static_cast<std::uint8_t>(value & 0xFFu);
}

void WriteU32(std::uint8_t* output, std::size_t offset, std::uint32_t value) noexcept {
  output[offset] = static_cast<std::uint8_t>((value >> 24u) & 0xFFu);
  output[offset + 1u] = static_cast<std::uint8_t>((value >> 16u) & 0xFFu);
  output[offset + 2u] = static_cast<std::uint8_t>((value >> 8u) & 0xFFu);
  output[offset + 3u] = static_cast<std::uint8_t>(value & 0xFFu);
}

bool IsSupportedType(std::uint8_t message_type) noexcept {
  return message_type == 0x00u || message_type == 0x01u || message_type == 0x02u ||
         message_type == 0x80u || message_type == 0x81u;
}

bool IsForbiddenReturnCode(std::uint8_t message_type, std::uint8_t return_code) noexcept {
  if (return_code != 0u) {
    return false;
  }
  return message_type == 0x81u;
}

bool RequiresZeroReturnCode(std::uint8_t message_type) noexcept {
  return message_type == 0x00u || message_type == 0x01u || message_type == 0x02u;
}

}  // namespace

RequestRegistry::RequestRegistry() noexcept : magic_(0x52455152u) {}

RequestRegistry::~RequestRegistry() noexcept = default;

bool RequestRegistry::IsActive(std::uint16_t clientId) const noexcept {
  return clientId != 0u;
}

bool RequestRegistry::IsActiveClient() const noexcept { return magic_ == 0x52455152u; }

Status decode_header(ByteView input, Header& out, std::size_t& frame_bytes) noexcept {
  if (input.IsNullRange()) {
    return Status::Error(ModuleCore, NullRange);
  }
  if (input.size < kSomeIpHeaderBytes) {
    return Status::Error(ModuleCore, NeedMore);
  }
  Header header;
  header.service_id = ReadU16(input, 0u);
  header.method_id = ReadU16(input, 2u);
  header.length = ReadU32(input, 4u);
  header.client_id = ReadU16(input, 8u);
  header.session_id = ReadU16(input, 10u);
  header.protocol_version = input.data[12u];
  header.interface_version = input.data[13u];
  header.message_type = input.data[14u];
  header.return_code = input.data[15u];
  out = header;
  frame_bytes = static_cast<std::size_t>(header.length) + 8u;
  return Status::Success();
}

Status encode_header(const Header& header, std::size_t payload_bytes, MutableByteView output,
                     std::size_t& written) noexcept {
  if (output.IsNullRange()) {
    return Status::Error(ModuleCore, NullRange);
  }
  if (output.size < kSomeIpHeaderBytes) {
    return Status::Error(ModuleCore, OutputCapacity);
  }
  WriteU16(output.data, 0u, header.service_id);
  WriteU16(output.data, 2u, header.method_id);
  WriteU32(output.data, 4u, header.length);
  WriteU16(output.data, 8u, header.client_id);
  WriteU16(output.data, 10u, header.session_id);
  output.data[12u] = header.protocol_version;
  output.data[13u] = header.interface_version;
  output.data[14u] = header.message_type;
  output.data[15u] = header.return_code;
  written = kSomeIpHeaderBytes;
  return Status::Success();
}

Status decode_message(ByteView input, const InterfaceDescriptor& descriptor,
                      DecodedValue& out) noexcept {
  (void)descriptor;
  Header header;
  std::size_t frame_bytes = 0u;
  const Status status = decode_header(input, header, frame_bytes);
  if (!status.IsOk()) {
    return status;
  }
  DecodedValue decoded;
  decoded.header = header;
  decoded.value.kind = ValueKind::ByteArray;
  decoded.value.bytes.assign(input.data + kSomeIpHeaderBytes, input.data + input.size);
  out = decoded;
  return Status::Success();
}

Status encode_message(const Header& header, const InterfaceDescriptor& descriptor,
                      const Value& value, MutableByteView output, std::size_t& written) noexcept {
  (void)descriptor;
  if (output.IsNullRange()) {
    return Status::Error(ModuleCore, NullRange);
  }
  Header local = header;
  local.length = static_cast<std::uint32_t>(value.bytes.size() + 8u);
  std::size_t header_written = 0u;
  const Status status = encode_header(local, value.bytes.size(), output, header_written);
  if (!status.IsOk()) {
    return status;
  }
  if (output.size < header_written + value.bytes.size()) {
    return Status::Error(ModuleCore, OutputCapacity);
  }
  for (std::size_t index = 0u; index < value.bytes.size(); ++index) {
    output.data[header_written + index] = value.bytes[index];
  }
  written = header_written + value.bytes.size();
  return Status::Success();
}

Status validate_message(const Header& header, const ElementDescriptor& element,
                        const RequestRegistry& registry, ValidationDecision& out) noexcept {
  ValidationDecision decision;
  if (header.protocol_version != kProtocolVersion) {
    decision.kind = ValidationRejected;
    decision.error = ProtocolVersion;
    out = decision;
    return Status::Error(ModuleCore, ProtocolVersion);
  }
  if (header.service_id == 0u || header.service_id == 0xFFFEu || header.service_id == 0xFFFFu) {
    decision.kind = ValidationRejected;
    decision.error = UnknownService;
    out = decision;
    return Status::Error(ModuleCore, UnknownService);
  }
  if (!IsSupportedType(header.message_type)) {
    decision.kind = ValidationRejected;
    decision.error = MessageType;
    out = decision;
    return Status::Error(ModuleCore, MessageType);
  }
  const bool type_and_role_consistent =
      (header.message_type == 0x00u) ||
      (header.message_type == 0x01u && !element.request_response) ||
      (header.message_type == 0x02u && (element.role == RoleEvent || element.role == RoleField)) ||
      ((header.message_type == 0x80u || header.message_type == 0x81u) &&
       (element.role == RoleMethodResponse || element.role == RoleMethodRequest));
  if (!type_and_role_consistent) {
    decision.kind = ValidationRejected;
    decision.error = MessageType;
    out = decision;
    return Status::Error(ModuleCore, MessageType);
  }
  if (IsForbiddenReturnCode(header.message_type, header.return_code) ||
      (RequiresZeroReturnCode(header.message_type) && header.return_code != 0u)) {
    decision.kind = ValidationRejected;
    decision.error = MessageType;
    out = decision;
    return Status::Error(ModuleCore, MessageType);
  }
  if (header.message_type == 0x80u || header.message_type == 0x81u) {
    if (!registry.IsActive(header.client_id)) {
      decision.kind = ValidationDropped;
      decision.error = UnknownRequest;
      out = decision;
      return Status::Error(ModuleCore, UnknownRequest);
    }
  }
  decision.kind = ValidationAccepted;
  out = decision;
  return Status::Success();
}

}  // namespace autoeth