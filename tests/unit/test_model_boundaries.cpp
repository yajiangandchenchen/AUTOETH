// AUTOETH unit tests: boundary behaviour identified during model checking.
//
// The model checker exercised CheckedAdd/CheckedMul/CheckedSub, CRC8 and the
// wide-to-classic narrowing. The cases below pin the boundaries it made
// explicit: exact representable limits, one-past-the-limit, and the
// documented "output unchanged on failure" rule (R0.3.3).
#include <cstddef>
#include <cstdint>
#include <vector>

#include "autoeth/classic/Crc.h"
#include "autoeth/classic/E2E.h"
#include "autoeth/check.hpp"
#include "autoeth/e2e.hpp"
#include "autoeth/detail/bit_geometry.hpp"
#include "test_check.hpp"

namespace {

const std::uint64_t kMaxU64 = ~static_cast<std::uint64_t>(0u);

// R0.2.1: bounds are checked before the operation, so the exact limit succeeds
// and one past it fails without touching the output.
void TestCheckedAddBoundary() {
  std::uint64_t out = 0u;
  AUTOETH_CHECK(autoeth::CheckedAdd(kMaxU64 - 1u, 1u, out));
  AUTOETH_CHECK(out == kMaxU64);
  out = 42u;
  AUTOETH_CHECK(!autoeth::CheckedAdd(kMaxU64, 1u, out));
  AUTOETH_CHECK(out == 42u);  // unchanged after failure (R0.3.3)
  AUTOETH_CHECK(autoeth::CheckedAdd(0u, 0u, out) && out == 0u);
}

void TestCheckedSubBoundary() {
  std::uint64_t out = 0u;
  AUTOETH_CHECK(autoeth::CheckedSub(kMaxU64, kMaxU64, out) && out == 0u);
  out = 7u;
  AUTOETH_CHECK(!autoeth::CheckedSub(0u, 1u, out));
  AUTOETH_CHECK(out == 7u);
}

void TestCheckedMulBoundary() {
  std::uint64_t out = 0u;
  // kMaxU64 / 2 is the largest factor that still fits when doubled.
  const std::uint64_t half = kMaxU64 / 2u;
  AUTOETH_CHECK(autoeth::CheckedMul(half, 2u, out));
  AUTOETH_CHECK(out == half * 2u);
  out = 9u;
  AUTOETH_CHECK(!autoeth::CheckedMul(half + 1u, 2u, out));
  AUTOETH_CHECK(out == 9u);
  // Zero operands never overflow.
  AUTOETH_CHECK(autoeth::CheckedMul(0u, kMaxU64, out) && out == 0u);
  AUTOETH_CHECK(autoeth::CheckedMul(kMaxU64, 0u, out) && out == 0u);
}

// CRC8 reference vector: the AUTOSAR CRC8/SAE-J1850 check value over
// "123456789" with init 0xFF and final XOR 0xFF is 0x4B.
void TestCrc8ReferenceVector() {
  const char* digits = "123456789";
  const uint8* data = reinterpret_cast<const uint8*>(digits);
  AUTOETH_CHECK(Crc_CalculateCRC8(data, 9u, 0u, TRUE) == 0x4Bu);
}

// The helper rejects a null buffer with a non-zero length instead of reading it.
void TestCrc8NullBufferWithLength() {
  AUTOETH_CHECK(Crc_CalculateCRC8(NULL_PTR, 4u, 0u, TRUE) == 0u);
}

// Bit geometry: the shift must equal the division for every legal width
// (MISRA C++ 1-0-3 deviation documented in check.cpp).
void TestBitGeometry() {
  AUTOETH_CHECK(autoeth::detail::BitsToBytes(8u) == 1u);
  AUTOETH_CHECK(autoeth::detail::BitsToBytes(16u) == 2u);
  AUTOETH_CHECK(autoeth::detail::BitsToBytes(32u) == 4u);
  AUTOETH_CHECK(autoeth::detail::BitsToBytes(64u) == 8u);
  AUTOETH_CHECK(autoeth::detail::BitsToBytes(0u) == 0u);
}

// Wide geometry that cannot narrow must report UNREPRESENTABLE and keep the
// classic output untouched (R6.6).
void TestWideNarrowingKeepsOutput() {
  autoeth::e2e::P08ConfigWide wide;
  wide.data_id = 3u;
  wide.max_data_length_bits = 8ull * 536870912ull;  // one past the classic limit
  wide.min_data_length_bits = 8ull * 16ull;
  wide.offset_bits = 0ull;
  wide.max_delta_counter = 1u;
  E2E_P08ConfigType classic;
  classic.DataID = 0xBEEFu;
  classic.MaxDataLength = 77u;
  AUTOETH_CHECK(autoeth::e2e::to_classic_p08_config(wide, classic)
                    .IsErr(autoeth::UnrepresentableClassicBitLength));
  AUTOETH_CHECK(classic.DataID == 0xBEEFu);
  AUTOETH_CHECK(classic.MaxDataLength == 77u);
}

// E2E counter domains stay separate: P02 pre-increments, P01 post-increments.
void TestE2ECounterFirstValue() {
  std::vector<uint8> p02(16u, 0u);
  E2E_P02ProtectStateType s02;
  E2E_P02ConfigType c02;
  AUTOETH_CHECK(E2E_P02ProtectInit(&s02) == E2E_E_OK);
  AUTOETH_CHECK(E2E_P02Protect(&c02, &s02, p02.data()) == E2E_E_OK);
  AUTOETH_CHECK(p02[8] == 1u && s02.Counter == 1u);

  std::vector<uint8> p01(16u, 0u);
  E2E_P01ProtectStateType s01;
  E2E_P01ConfigType c01;
  AUTOETH_CHECK(E2E_P01ProtectInit(&s01) == E2E_E_OK);
  AUTOETH_CHECK(E2E_P01Protect(&c01, &s01, p01.data()) == E2E_E_OK);
  AUTOETH_CHECK(p01[8] == 0u && s01.Counter == 1u);
}

}  // namespace

void RunModelCheckingDerivedTests() {
  TestCheckedAddBoundary();
  TestCheckedSubBoundary();
  TestCheckedMulBoundary();
  TestCrc8ReferenceVector();
  TestCrc8NullBufferWithLength();
  TestBitGeometry();
  TestWideNarrowingKeepsOutput();
  TestE2ECounterFirstValue();
}