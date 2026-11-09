// AUTOETH core: TP segmentation and reassembly. R5.1 fixed strategy, R5.3
// min-before-align upper bound and R5.5 expiry ordering.
#include "autoeth/tp.hpp"

#include <algorithm>
#include <vector>

namespace autoeth {
namespace core_detail {

const std::size_t kTpHeaderBytes = 20u;

std::uint32_t SegmentLengthOf(ByteView frame) noexcept {
  if (frame.size < 8u) {
    return 0u;
  }
  const std::uint32_t b0 = frame.data[4u];
  const std::uint32_t b1 = frame.data[5u];
  const std::uint32_t b2 = frame.data[6u];
  const std::uint32_t b3 = frame.data[7u];
  return (b0 << 24u) | (b1 << 16u) | (b2 << 8u) | b3;
}

std::uint32_t SegmentOffsetOf(ByteView frame) noexcept {
  const std::uint32_t b0 = frame.data[16u];
  const std::uint32_t b1 = frame.data[17u];
  const std::uint32_t b2 = frame.data[18u];
  const std::uint32_t b3 = frame.data[19u];
  return (b0 << 24u) | (b1 << 16u) | (b2 << 8u) | b3;
}

bool SegmentMoreFlag(ByteView frame) noexcept {
  return (frame.data[15u] & 0x01u) != 0u;
}

struct Assembly {
  std::uint32_t final_end;
  bool started;
  std::vector<std::uint8_t> buffer;
  std::vector<bool> covered;
  std::uint64_t deadline;
  std::uint8_t last_return_code;
  std::uint64_t generation;
};

}  // namespace core_detail
using core_detail::kTpHeaderBytes;
using core_detail::Assembly;

using core_detail::SegmentLengthOf;
using core_detail::SegmentOffsetOf;
using core_detail::SegmentMoreFlag;

std::size_t NonFinalSegmentPayloadBytes(std::uint32_t configured_max,
                                        std::uint32_t datagram_capacity) noexcept {
  if (datagram_capacity < kTpHeaderBytes) {
    return 0u;
  }
  const std::uint32_t available = datagram_capacity - static_cast<std::uint32_t>(kTpHeaderBytes);
  const std::uint32_t raw = std::min(configured_max, available);
  // Divide by 16 and rescale with shifts: 16 is a power of two and raw is
  // unsigned, so this is exact and independent of division rounding (1-0-3).
  const std::uint32_t aligned = (raw >> 4u) << 4u;
  return static_cast<std::size_t>(aligned);
}

SegmentSink::SegmentSink() noexcept : reserved_(0u) {}

SegmentSink::~SegmentSink() noexcept = default;

bool SegmentSink::ReserveBurst(std::size_t total_bytes) noexcept {
  reserved_ = total_bytes;
  return true;
}

std::size_t SegmentSink::ReservedBytes() const noexcept { return reserved_; }

ExpirySink::ExpirySink() noexcept : cancel_count(0u) {}

ExpirySink::~ExpirySink() noexcept = default;

void ExpirySink::OnCancelled(std::uint32_t session, std::uint64_t now_ns) noexcept {
  (void)session;
  (void)now_ns;
  ++cancel_count;
}

struct TpReceiver::Impl {
  std::vector<Assembly> assemblies;
  std::vector<std::uint32_t> discarded_session;
  std::vector<std::uint64_t> discarded_deadline;
  std::uint64_t generation;
};

TpReceiver::TpReceiver() noexcept : impl_(new Impl()) { impl_->generation = 1u; }

TpReceiver::~TpReceiver() noexcept = default;

std::size_t TpReceiver::AssemblyCount() const noexcept { return impl_->assemblies.size(); }

Status segment(const Message& message, const TpTxConfig& config, SegmentSink& sink) noexcept {
  const std::size_t nonfinal = NonFinalSegmentPayloadBytes(config.max_segment_payload_bytes,
                                                            config.datagram_capacity_bytes);
  if (nonfinal < 16u) {
    return Status::Error(ModuleTp, InvalidConfig);
  }
  const std::size_t payload = message.payload.size();
  std::size_t total = 0u;
  std::size_t remaining = payload;
  while (remaining > 0u) {
    const std::size_t chunk = (remaining > nonfinal) ? nonfinal : remaining;
    total += kTpHeaderBytes + chunk;
    remaining -= chunk;
  }
  if (payload == 0u) {
    total = kTpHeaderBytes;
  }
  if (!sink.ReserveBurst(total)) {
    return Status::Error(ModuleTp, Resource);
  }
  return Status::Success();
}

Status TpReceiver::accept(const Peer& peer, ByteView wire_frame, std::uint64_t now_ns,
                          ReceiveOutcome& out) noexcept {
  (void)peer;
  ReceiveOutcome result;
  result.classification = TpIgnored;
  if (wire_frame.IsNullRange()) {
    result.diagnostic = NullRange;
    out = result;
    return Status::Error(ModuleTp, NullRange);
  }
  if (wire_frame.size < kTpHeaderBytes) {
    result.diagnostic = LengthTooSmall;
    out = result;
    return Status::Error(ModuleTp, LengthTooSmall);
  }
  for (std::size_t index = 0u; index < impl_->assemblies.size(); ++index) {
    if (now_ns >= impl_->assemblies[index].deadline) {
      impl_->assemblies.erase(impl_->assemblies.begin() + static_cast<std::ptrdiff_t>(index));
      result.classification = TpCancelled;
      out = result;
      return Status::Success();
    }
  }
  const std::uint32_t total_length = SegmentLengthOf(wire_frame);
  const std::uint32_t offset = SegmentOffsetOf(wire_frame);
  const std::size_t segment_bytes = wire_frame.size - kTpHeaderBytes;
  if (offset > total_length || segment_bytes > static_cast<std::size_t>(total_length - offset)) {
    result.diagnostic = PayloadMalformed;
    out = result;
    return Status::Success();
  }
  const std::uint32_t final_end = offset + static_cast<std::uint32_t>(segment_bytes);
  bool found = false;
  std::size_t slot = 0u;
  for (std::size_t index = 0u; index < impl_->assemblies.size(); ++index) {
    if (!impl_->assemblies[index].buffer.empty() &&
        impl_->assemblies[index].buffer.size() == total_length) {
      found = true;
      slot = index;
      break;
    }
  }
  if (!found) {
    Assembly assembly;
    assembly.final_end = final_end;
    assembly.started = true;
    assembly.buffer.assign(total_length, 0u);
    assembly.covered.assign(total_length, false);
    assembly.deadline = now_ns + 1000000000u;
    assembly.last_return_code = wire_frame.data[15u];
    assembly.generation = impl_->generation++;
    impl_->assemblies.push_back(assembly);
    slot = impl_->assemblies.size() - 1u;
  }
  Assembly& assembly = impl_->assemblies[slot];
  for (std::size_t index = 0u; index < segment_bytes; ++index) {
    assembly.buffer[offset + index] = wire_frame.data[kTpHeaderBytes + index];
    assembly.covered[offset + index] = true;
  }
  assembly.last_return_code = wire_frame.data[15u];
  assembly.deadline = now_ns + 1000000000u;
  if (!SegmentMoreFlag(wire_frame) && assembly.final_end >= offset + segment_bytes) {
    bool complete = true;
    for (std::size_t index = 0u; index < assembly.covered.size(); ++index) {
      if (!assembly.covered[index]) {
        complete = false;
        break;
      }
    }
    if (complete) {
      result.classification = TpAccepted;
      result.has_message = true;
      result.message.payload = assembly.buffer;
      result.message.header.return_code = assembly.last_return_code;
      impl_->assemblies.erase(impl_->assemblies.begin() + static_cast<std::ptrdiff_t>(slot));
    }
  }
  out = result;
  return Status::Success();
}

void TpReceiver::expire(std::uint64_t now_ns, ExpirySink& sink) noexcept {
  std::size_t index = 0u;
  while (index < impl_->assemblies.size()) {
    if (now_ns >= impl_->assemblies[index].deadline) {
      sink.OnCancelled(0u, now_ns);
      impl_->assemblies.erase(impl_->assemblies.begin() + static_cast<std::ptrdiff_t>(index));
    } else {
      ++index;
    }
  }
}

void TpReceiver::reset(TpResetReason reason) noexcept {
  (void)reason;
  impl_->assemblies.clear();
  impl_->discarded_session.clear();
  impl_->discarded_deadline.clear();
  ++impl_->generation;
}

}  // namespace autoeth