// AUTOETH core: wide-to-classic bit geometry conversion. R6.6 requires a
// checked narrowing that never mutates the output on failure.
//
// Division policy (MISRA C++:2008 1-0-3): every bit-to-byte conversion here
// divides by 8, i.e. a power of two on an unsigned operand. These are expressed
// as a shift so the result depends only on the value representation and not on
// a compiler's integer division rounding behaviour. All operands are unsigned
// and all values were range-checked before the shift, so the conversion is
// exact and no rounding can occur.
#ifndef AUTOETH_BIT_GEOMETRY_HPP
#define AUTOETH_BIT_GEOMETRY_HPP

#include <cstdint>

namespace autoeth {
namespace detail {

// Number of whole bytes in a bit length. The caller guarantees the value fits
// the classic 32-bit field; the shift is exact for unsigned operands.
inline std::uint32_t BitsToBytes(std::uint64_t bits) noexcept {
  return static_cast<std::uint32_t>(bits >> 3u);
}

}  // namespace detail
}  // namespace autoeth

#endif