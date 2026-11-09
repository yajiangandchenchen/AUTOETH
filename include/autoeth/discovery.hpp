// AUTOETH Native discovery surface. R7.1 raw SD codec, R7.19 selection.
#ifndef AUTOETH_DISCOVERY_HPP
#define AUTOETH_DISCOVERY_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "autoeth/codec.hpp"
#include "autoeth/types.hpp"

namespace autoeth {

enum SdMessageType : std::uint8_t {
  SdFindService = 0x00,
  SdOfferService = 0x01,
  SdSubscribeEventgroup = 0x06,
  SdSubscribeEventgroupAck = 0x07
};

struct SdEndpoint {
  std::uint8_t address[16];
  std::uint16_t port;
  std::uint8_t address_family;

  SdEndpoint() noexcept : port(0u), address_family(0u) { address[0] = 0u; }
};

struct SdOfferSnapshot {
  std::uint16_t service_id;
  std::uint16_t instance_id;
  std::uint8_t major_version;
  std::uint32_t minor_unused;
  std::uint32_t ttl;

  SdOfferSnapshot() noexcept
      : service_id(0u), instance_id(0u), major_version(0u), minor_unused(0u), ttl(0u) {}
};

struct SdMessage {
  SdMessageType type;
  std::uint8_t address_family;
  std::vector<std::uint8_t> raw;

  SdMessage() noexcept : type(SdFindService), address_family(0u) {}
};

enum SdEventKind : std::uint8_t {
  SdIngress = 0,
  SdServiceRequest = 1,
  SdServiceRelease = 2,
  SdAvailability = 3,
  SdLinkIp = 4,
  SdConnection = 5,
  SdTimer = 6
};

struct SdEvent {
  SdEventKind kind;
  std::uint64_t value;

  SdEvent() noexcept : kind(SdIngress), value(0u) {}
};

enum ActionKind : std::uint8_t {
  ActionSendSd = 0,
  ActionRoutingChange = 1,
  ActionConnectionRequest = 2,
  ActionCurrentStateChange = 3,
  ActionFieldInitial = 4,
  ActionDiagnostic = 5
};

struct Action {
  ActionKind kind;
  std::uint32_t target;
  std::vector<std::uint8_t> bytes;

  Action() noexcept : kind(ActionSendSd), target(0u) {}
};

class ActionBatch {
 public:
  ActionBatch() noexcept;
  ~ActionBatch() noexcept;
  ActionBatch(const ActionBatch&) = delete;
  ActionBatch& operator=(const ActionBatch&) = delete;

  bool Reserve(std::size_t count) noexcept;
  bool Append(const Action& action) noexcept;
  std::size_t Size() const noexcept;
  const Action& At(std::size_t index) const noexcept;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

class RandomDraw {
 public:
  RandomDraw() noexcept;
  virtual ~RandomDraw() noexcept;
  RandomDraw(const RandomDraw&) = delete;
  RandomDraw& operator=(const RandomDraw&) = delete;

  virtual std::uint64_t Next(std::uint64_t bound) noexcept;
};

struct CapabilityCriterion {
  std::vector<std::uint8_t> key;
  std::vector<std::uint8_t> value;
  ValuePresence presence;

  CapabilityCriterion() noexcept : presence(NoEquals) {}
};

struct ServiceQuery {
  std::uint32_t interface_id;
  std::uint16_t service_id;
  std::uint16_t instance_id;
  std::uint8_t major_version;
  std::uint32_t minor_version;
  bool exact_instance;
  std::uint8_t transport;
  std::vector<CapabilityCriterion> capabilities;

  ServiceQuery() noexcept
      : interface_id(0u), service_id(0u), instance_id(0u), major_version(0u), minor_version(0u),
        exact_instance(false), transport(0u) {}
};

class SdEngine {
 public:
  SdEngine() noexcept;
  ~SdEngine() noexcept;
  SdEngine(const SdEngine&) = delete;
  SdEngine& operator=(const SdEngine&) = delete;

  Status dispatch(const SdEvent& event, std::uint64_t now_ns, ActionBatch& out) noexcept;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

Status decode_sd_message(ByteView input, SdMessage& out) noexcept;
Status encode_sd_message(const SdMessage& message, MutableByteView output,
                         std::size_t& written) noexcept;
Status select_service(const ServiceQuery& query, const std::vector<SdOfferSnapshot>& offers,
                      RandomDraw& draw, SdOfferSnapshot& out) noexcept;

bool IsActionableEndpoint(const SdEndpoint& endpoint) noexcept;

}  // namespace autoeth

#endif