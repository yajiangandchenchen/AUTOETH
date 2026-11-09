// AUTOETH Native wire surface. R3.1 header fields, R3.3 validation order.
#ifndef AUTOETH_WIRE_HPP
#define AUTOETH_WIRE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "autoeth/codec.hpp"
#include "autoeth/types.hpp"

namespace autoeth {

static const std::size_t kSomeIpHeaderBytes = 16u;
static const std::size_t kSomeIpTpHeaderBytes = 20u;
static const std::uint8_t kProtocolVersion = 1u;

struct Message {
  Header header;
  std::vector<std::uint8_t> payload;

  Message() = default;
};

struct DecodedValue {
  Header header;
  Value value;

  DecodedValue() = default;
};

struct InterfaceDescriptor {
  std::uint16_t service_id;
  std::uint8_t major_version;

  InterfaceDescriptor() noexcept : service_id(0u), major_version(0u) {}
};

struct ElementDescriptor {
  std::uint16_t method_id;
  SchemaRole role;
  bool request_response;

  ElementDescriptor() noexcept : method_id(0u), role(RoleEvent), request_response(false) {}
};

class RequestRegistry {
 public:
  RequestRegistry() noexcept;
  ~RequestRegistry() noexcept;
  RequestRegistry(const RequestRegistry&) = delete;
  RequestRegistry& operator=(const RequestRegistry&) = delete;
  RequestRegistry(RequestRegistry&&) = delete;
  RequestRegistry& operator=(RequestRegistry&&) = delete;

  bool IsActive(std::uint16_t clientId) const noexcept;
  bool IsActiveClient() const noexcept;

 private:
  std::uint32_t magic_;
};

enum ValidationKind : std::uint8_t {
  ValidationAccepted = 0,
  ValidationRejected = 1,
  ValidationErrorReply = 2,
  ValidationDropped = 3
};

struct ValidationDecision {
  ValidationKind kind;
  Errc error;
  Header reply;

  ValidationDecision() noexcept : kind(ValidationRejected), error(Ok) {}
};

Status decode_header(ByteView input, Header& out, std::size_t& frame_bytes) noexcept;
Status encode_header(const Header& header, std::size_t payload_bytes, MutableByteView output,
                     std::size_t& written) noexcept;
Status decode_message(ByteView input, const InterfaceDescriptor& descriptor,
                      DecodedValue& out) noexcept;
Status encode_message(const Header& header, const InterfaceDescriptor& descriptor,
                      const Value& value, MutableByteView output, std::size_t& written) noexcept;
Status validate_message(const Header& header, const ElementDescriptor& element,
                        const RequestRegistry& registry, ValidationDecision& out) noexcept;

}  // namespace autoeth

#endif