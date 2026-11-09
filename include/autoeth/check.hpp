// AUTOETH core: checked arithmetic declarations. INV-3 requires every length
// and offset computation to flow through these helpers.
#ifndef AUTOETH_CHECK_HPP
#define AUTOETH_CHECK_HPP

#include <cstdint>

namespace autoeth {

bool CheckedAdd(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;
bool CheckedMul(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;
bool CheckedSub(std::uint64_t lhs, std::uint64_t rhs, std::uint64_t& out) noexcept;

}  // namespace autoeth

#endif