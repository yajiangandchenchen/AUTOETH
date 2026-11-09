// AUTOETH core: checked arithmetic helpers. INV-3 / R0.3.3.
//
// Integer division policy (MISRA C++:2008 1-0-3, "the implementation of integer
// division in the chosen compiler shall be determined and documented"):
//   - This translation unit performs integer division in exactly one place,
//     DivideMaxBy, and only on unsigned 64-bit operands.
//   - On unsigned operands C++14 [expr.mul]/3 defines the result as the
//     algebraic quotient with any fractional part discarded. There is no
//     rounding choice to make and no negative operand, so the two compiler
//     behaviours the rule warns about cannot arise.
//   - Operands of different signedness are never divided here; the helper is
//     declared only for std::uint64_t (see check.hpp).
//   - The platform is fixed by R0.4.1 (GCC 13, two's complement host), and the
//     division is documented here for the developer of this unit.
#include "autoeth/check.hpp"

namespace autoeth {

// The only integer division in this translation unit. It is separated so the
// documented division policy at the top of the file applies to exactly one
// site. max/divisor is the largest factor for which divisor*factor does not
// overflow.
namespace core_detail {

std::uint64_t DivideMaxBy(std::uint64_t divisor) noexcept {
  // MISRA C++:2008 1-0-3 deviation, documented by the policy at the top of
  // this file: the operands are unsigned, so [expr.mul]/3 fully defines the
  // quotient and the signed rounding choice the rule warns about cannot occur.
  // Deviation record: docs/08_实施/静态分析偏差单.md
  return static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0u)) / divisor;
}

}  // namespace core_detail

using core_detail::DivideMaxBy;

bool CheckedAdd(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept {
  if (lhs > static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0u)) - rhs) {
    return false;
  }
  out = lhs + rhs;
  return true;
}

bool CheckedMul(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept {
  // No integer division at this site: with lhs != 0 the product overflows
  // exactly when rhs exceeds the largest factor that keeps it representable.
  // That bound is computed once in DivideMaxBy, whose unsigned division policy
  // is documented at the top of this file (MISRA C++:2008 1-0-3).
  if (lhs != 0u && rhs > DivideMaxBy(lhs)) {
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