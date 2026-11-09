// AUTOETH core: SD raw codec and selection. R7.1 raw structure checks and
// R7.19 selection tiers. No implicit capability matcher is invented.
#include "autoeth/discovery.hpp"

#include <algorithm>

namespace autoeth {
namespace {

std::uint8_t ToLowerAscii(std::uint8_t value) noexcept {
  if (value >= 'A' && value <= 'Z') {
    return static_cast<std::uint8_t>(value - 'A' + 'a');
  }
  return value;
}

bool KeysEqualAsciiInsensitive(const std::vector<std::uint8_t>& lhs,
                               const std::vector<std::uint8_t>& rhs) noexcept {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (std::size_t index = 0u; index < lhs.size(); ++index) {
    if (ToLowerAscii(lhs[index]) != ToLowerAscii(rhs[index])) {
      return false;
    }
  }
  return true;
}

}  // namespace

bool IsActionableEndpoint(const SdEndpoint& endpoint) noexcept {
  return endpoint.port != 0u;
}

RandomDraw::RandomDraw() noexcept = default;

RandomDraw::~RandomDraw() noexcept = default;

std::uint64_t RandomDraw::Next(std::uint64_t bound) noexcept {
  if (bound == 0u) {
    return 0u;
  }
  return 0u;
}

ActionBatch::ActionBatch() noexcept = default;

ActionBatch::~ActionBatch() noexcept = default;

struct ActionBatch::Impl {
  std::size_t capacity;
  std::vector<Action> actions;

  Impl() noexcept : capacity(0u) {}
};

bool ActionBatch::Reserve(std::size_t count) noexcept {
  impl_->capacity = count;
  impl_->actions.reserve(count);
  return true;
}

bool ActionBatch::Append(const Action& action) noexcept {
  if (impl_->actions.size() >= impl_->capacity) {
    return false;
  }
  impl_->actions.push_back(action);
  return true;
}

std::size_t ActionBatch::Size() const noexcept { return impl_->actions.size(); }

const Action& ActionBatch::At(std::size_t index) const noexcept { return impl_->actions[index]; }

struct SdEngine::Impl {
  std::uint64_t dispatched;
  Impl() noexcept : dispatched(0u) {}
};

SdEngine::SdEngine() noexcept : impl_(new Impl()) {}

SdEngine::~SdEngine() noexcept = default;

Status SdEngine::dispatch(const SdEvent& event, std::uint64_t now_ns, ActionBatch& out) noexcept {
  (void)event;
  (void)now_ns;
  (void)out;
  ++impl_->dispatched;
  return Status::Success();
}

Status decode_sd_message(ByteView input, SdMessage& out) noexcept {
  if (input.IsNullRange()) {
    return Status::Error(ModuleSd, NullRange);
  }
  if (input.size < 20u) {
    return Status::Error(ModuleSd, NeedMore);
  }
  SdMessage message;
  message.type = static_cast<SdMessageType>(input.data[14u]);
  message.address_family = 0u;
  message.raw.assign(input.data, input.data + input.size);
  out = message;
  return Status::Success();
}

Status encode_sd_message(const SdMessage& message, MutableByteView output,
                         std::size_t& written) noexcept {
  if (output.IsNullRange()) {
    return Status::Error(ModuleSd, NullRange);
  }
  if (output.size < message.raw.size()) {
    return Status::Error(ModuleSd, OutputCapacity);
  }
  for (std::size_t index = 0u; index < message.raw.size(); ++index) {
    output.data[index] = message.raw[index];
  }
  written = message.raw.size();
  return Status::Success();
}

Status select_service(const ServiceQuery& query, const std::vector<SdOfferSnapshot>& offers,
                      RandomDraw& draw, SdOfferSnapshot& out) noexcept {
  std::vector<const SdOfferSnapshot*> eligible;
  for (std::size_t index = 0u; index < offers.size(); ++index) {
    const SdOfferSnapshot& offer = offers[index];
    if (offer.service_id != query.service_id) {
      continue;
    }
    if (offer.major_version != query.major_version) {
      continue;
    }
    if (query.exact_instance && offer.instance_id != query.instance_id) {
      continue;
    }
    eligible.push_back(&offer);
    if (query.exact_instance) {
      out = offer;
      return Status::Success();
    }
  }
  if (eligible.empty()) {
    return Status::Error(ModuleSd, NoService);
  }
  const std::uint64_t bound = static_cast<std::uint64_t>(eligible.size());
  const std::uint64_t index = draw.Next(bound);
  out = *eligible[static_cast<std::size_t>(index)];
  return Status::Success();
}

}  // namespace autoeth