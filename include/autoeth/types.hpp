// AUTOETH Native public types: ByteView, Status, Errc, Header and handle aggregates.
// R0.3.3 / R8.3: Status is allocation free and handles are Runtime local.
#ifndef AUTOETH_TYPES_HPP
#define AUTOETH_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace autoeth {

struct ByteView {
  const std::uint8_t* data;
  std::size_t size;

  ByteView() noexcept : data(nullptr), size(0) {}
  ByteView(const std::uint8_t* dataIn, std::size_t sizeIn) noexcept : data(dataIn), size(sizeIn) {}

  bool IsNullRange() const noexcept { return data == nullptr && size != 0; }
};

struct MutableByteView {
  std::uint8_t* data;
  std::size_t size;

  MutableByteView() noexcept : data(nullptr), size(0) {}
  MutableByteView(std::uint8_t* dataIn, std::size_t sizeIn) noexcept : data(dataIn), size(sizeIn) {}

  bool IsNullRange() const noexcept { return data == nullptr && size != 0; }
  ByteView AsConst() const noexcept { return ByteView(data, size); }
};

struct DurationNs {
  std::uint64_t value;

  constexpr explicit DurationNs(std::uint64_t valueIn) noexcept : value(valueIn) {}
};

struct MonotonicNs {
  std::uint64_t value;

  constexpr explicit MonotonicNs(std::uint64_t valueIn) noexcept : value(valueIn) {}
};

enum Module : std::uint16_t {
  ModuleCore = 1,
  ModuleCodec = 2,
  ModuleTp = 3,
  ModuleE2e = 4,
  ModuleSd = 5,
  ModuleRuntime = 6,
  ModuleIo = 7,
  ModuleConfig = 8,
  ModuleApplication = 9
};

enum Errc : std::uint16_t {
  Ok = 0,
  NeedMore = 1,
  NullRange = 2,
  LengthTooSmall = 3,
  LengthOverflow = 4,
  OutputCapacity = 5,
  ProtocolVersion = 6,
  UnknownService = 7,
  InterfaceVersion = 8,
  UnknownMethod = 9,
  MessageType = 10,
  UnknownRequest = 11,
  PayloadMalformed = 12,
  InvalidConfig = 13,
  Resource = 14,
  Io = 15,
  Timeout = 16,
  Cancelled = 17,
  InvalidHandle = 18,
  AlreadyComplete = 19,
  InvalidState = 20,
  WouldDeadlock = 21,
  NoService = 22,
  UnsupportedProfile = 23,
  UnrepresentableClassicBitLength = 24,
  TransportAborted = 25,
  CleanupOverdue = 26,
  AssumptionViolation = 27,
  CounterExhausted = 28,
  HandlerFailure = 29,
  RemoteProtocol = 30,
  RemoteApplication = 31,
  NotReady = 32,
  VersionUnsupported = 33,
  ErrcCount = 34
};

struct Status {
  std::uint16_t module;
  std::uint32_t typed_code;
  std::uint64_t byte_offset;
  std::uint32_t detail;
  std::int32_t native_errno;

  Status() noexcept
      : module(0u), typed_code(static_cast<std::uint32_t>(Ok)), byte_offset(0u), detail(0u),
        native_errno(0) {}

  Status(Module moduleIn, Errc errc) noexcept
      : module(static_cast<std::uint16_t>(moduleIn)),
        typed_code(static_cast<std::uint32_t>(errc)), byte_offset(0u), detail(0u),
        native_errno(0) {}

  bool IsOk() const noexcept { return typed_code == static_cast<std::uint32_t>(Ok); }
  bool IsErr(Errc errc) const noexcept { return typed_code == static_cast<std::uint32_t>(errc); }

  static Status Success() noexcept { return Status(); }
  static Status Error(Module moduleIn, Errc errc) noexcept { return Status(moduleIn, errc); }
};

struct Header {
  std::uint16_t service_id;
  std::uint16_t method_id;
  std::uint16_t client_id;
  std::uint16_t session_id;
  std::uint32_t length;
  std::uint8_t protocol_version;
  std::uint8_t interface_version;
  std::uint8_t message_type;
  std::uint8_t return_code;

  Header() noexcept
      : service_id(0u), method_id(0u), client_id(0u), session_id(0u), length(0u),
        protocol_version(0u), interface_version(0u), message_type(0u), return_code(0u) {}
};

struct ServiceHandle {
  std::uint32_t slot;
  std::uint64_t generation;

  ServiceHandle() noexcept : slot(0u), generation(0u) {}
};

struct RequestHandle {
  std::uint32_t slot;
  std::uint64_t generation;

  RequestHandle() noexcept : slot(0u), generation(0u) {}
};

struct SubscriptionHandle {
  std::uint32_t slot;
  std::uint64_t generation;

  SubscriptionHandle() noexcept : slot(0u), generation(0u) {}
};

struct CallHandle {
  std::uint32_t slot;
  std::uint64_t generation;

  CallHandle() noexcept : slot(0u), generation(0u) {}

  bool operator==(const CallHandle& other) const noexcept {
    return slot == other.slot && generation == other.generation;
  }
};

}  // namespace autoeth

#endif