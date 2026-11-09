// AUTOETH Native security provider gate. R8.18. No credential material is
// loaded from JSON and no TLS/cryptography is implemented here.
#ifndef AUTOETH_SECURITY_HPP
#define AUTOETH_SECURITY_HPP

#include <cstdint>
#include <functional>

#include "autoeth/types.hpp"

namespace autoeth {

enum class SecurityState : std::uint8_t { Pending, Ready, Failed, Expired };

using SecurityCompletion = std::function<void(std::uint64_t, SecurityState)>;

struct SecurityRequest {
  std::uint32_t endpoint_index;
  std::uint32_t peer_address;
  std::uint64_t connection_generation;

  SecurityRequest() noexcept
      : endpoint_index(0u), peer_address(0u), connection_generation(0u) {}
};

class SecurityProvider {
 public:
  SecurityProvider() noexcept;
  virtual ~SecurityProvider() noexcept;
  SecurityProvider(const SecurityProvider&) = delete;
  SecurityProvider& operator=(const SecurityProvider&) = delete;

  virtual Status begin(const SecurityRequest& request, SecurityCompletion completion,
                       std::uint64_t& token) noexcept;
  virtual void cancel(std::uint64_t token) noexcept;
};

}  // namespace autoeth

#endif