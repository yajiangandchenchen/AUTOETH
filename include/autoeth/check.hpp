// AUTOETH core: checked arithmetic declarations. INV-3 requires every length
// and offset computation to flow through these helpers.
//
// Precondition (contract, not a runtime check):
//   `out` must refer to a valid, writable std::uint64_t object. It is a C++
//   reference, so null cannot be formed by conforming callers; the overloads
//   are internal to the SDK and every call site passes a local variable
//   (see src/core/codec.cpp). The helpers stay allocation free and branch
//   minimal because they run on every length computation.
//
// Return value: true when the result fits and was stored, false when the
// operation would overflow/underflow. On false the object referred to by `out`
// is left unchanged, which R0.3.3 requires for failed narrowing.
#ifndef AUTOETH_CHECK_HPP
#define AUTOETH_CHECK_HPP

#include <cstdint>

namespace autoeth {

bool CheckedAdd(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;
bool CheckedMul(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;
bool CheckedSub(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;

}  // namespace autoeth

#endif