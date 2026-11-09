// AUTOETH core: checked arithmetic helpers. INV-3 / R0.3.3.
#include "autoeth/check.hpp"

namespace autoeth {

bool CheckedAdd(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept {
  if (lhs > static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0u)) - rhs) {
    return false;
  }
  out = lhs + rhs;
  return true;
}

bool CheckedMul(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept {
  if (lhs != 0u && rhs > static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0u)) / lhs) {
    return false;
  }
  out = lhs * rhs;
  return true;
}

bool CheckedSub(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept {
  if (rhs > lhs) {
    return false;
  }
  out = lhs - rhs;
  return true;
}

}  // namespace autoeth