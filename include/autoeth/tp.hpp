// AUTOETH Native TP surface. R5.9 ReceiveOutcome, R5.1 fixed strategy.
#ifndef AUTOETH_TP_HPP
#define AUTOETH_TP_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "autoeth/types.hpp"
#include "autoeth/wire.hpp"

namespace autoeth {

enum TpStrategy : std::uint8_t { TpFoContiguous = 0, TpCpStreaming = 1 };

enum TpAcceptClass : std::uint8_t { TpAccepted = 0, TpIgnored = 1, TpCancelled = 2 };

enum TpResetReason : std::uint8_t {
  TpResetSession = 0,
  TpResetLinkLoss = 1,
  TpResetShutdown = 2
};

struct TpTxConfig {
  TpStrategy strategy;
  std::uint32_t max_segment_payload_bytes;
  std::uint64_t reassembly_timeout_ns;
  std::uint32_t datagram_capacity_bytes;

  TpTxConfig() noexcept
      : strategy(TpFoContiguous), max_segment_payload_bytes(1392u), reassembly_timeout_ns(0u),
        datagram_capacity_bytes(1472u) {}
};

struct Peer {
  std::uint32_t address;
  std::uint32_t interface_index;
  std::uint8_t transport;
  std::uint16_t client_id;

  Peer() noexcept : address(0u), interface_index(0u), transport(0u), client_id(0u) {}
};

struct ReceiveOutcome {
  TpAcceptClass classification;
  bool has_message;
  Message message;
  Errc diagnostic;

  ReceiveOutcome() noexcept : classification(TpAccepted), has_message(false), diagnostic(Ok) {}
};

class SegmentSink {
 public:
  SegmentSink() noexcept;
  virtual ~SegmentSink() noexcept;
  SegmentSink(const SegmentSink&) = delete;
  SegmentSink& operator=(const SegmentSink&) = delete;

  bool ReserveBurst(std::size_t total_bytes) noexcept;
  std::size_t ReservedBytes() const noexcept;

 private:
  std::size_t reserved_;
};

class ExpirySink {
 public:
  ExpirySink() noexcept;
  virtual ~ExpirySink() noexcept;
  ExpirySink(const ExpirySink&) = delete;
  ExpirySink& operator=(const ExpirySink&) = delete;

  void OnCancelled(std::uint32_t session, std::uint64_t now_ns) noexcept;

  std::uint32_t cancel_count;
};

class TpReceiver {
 public:
  TpReceiver() noexcept;
  ~TpReceiver() noexcept;
  TpReceiver(const TpReceiver&) = delete;
  TpReceiver& operator=(const TpReceiver&) = delete;

  std::size_t AssemblyCount() const noexcept;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

 public:
  Status accept(const Peer& peer, ByteView wire_frame, std::uint64_t now_ns,
                ReceiveOutcome& out) noexcept;
  void expire(std::uint64_t now_ns, ExpirySink& sink) noexcept;
  void reset(TpResetReason reason) noexcept;
};

Status segment(const Message& message, const TpTxConfig& config, SegmentSink& sink) noexcept;

std::size_t NonFinalSegmentPayloadBytes(std::uint32_t configured_max,
                                        std::uint32_t datagram_capacity) noexcept;

}  // namespace autoeth

#endif