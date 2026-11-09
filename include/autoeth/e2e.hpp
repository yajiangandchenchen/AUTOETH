// AUTOETH Native E2E surface: wide P08/P08m and state-machine configuration.
// R6.6 and R6.7. Source C types come from classic/E2E.h unchanged.
#ifndef AUTOETH_E2E_HPP
#define AUTOETH_E2E_HPP

#include <cstdint>

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/E2E.h"
#include "autoeth/types.hpp"

namespace autoeth {
namespace e2e {

struct P08ConfigWide {
  std::uint32_t data_id;
  std::uint64_t max_data_length_bits;
  std::uint32_t max_delta_counter;
  std::uint64_t min_data_length_bits;
  std::uint64_t offset_bits;

  P08ConfigWide() noexcept
      : data_id(0u), max_data_length_bits(0u), max_delta_counter(0u), min_data_length_bits(0u),
        offset_bits(0u) {}
};

struct P08mConfigWide {
  std::uint32_t data_id;
  std::uint64_t max_data_length_bits;
  std::uint32_t max_delta_counter;
  std::uint64_t min_data_length_bits;
  std::uint64_t offset_bits;

  P08mConfigWide() noexcept
      : data_id(0u), max_data_length_bits(0u), max_delta_counter(0u), min_data_length_bits(0u),
        offset_bits(0u) {}
};

struct SmConfig {
  std::uint8_t window_size_valid;
  std::uint8_t min_ok_state_init;
  std::uint8_t max_error_state_init;
  std::uint8_t min_ok_state_valid;
  std::uint8_t max_error_state_valid;
  std::uint8_t min_ok_state_invalid;
  std::uint8_t max_error_state_invalid;
  std::uint8_t window_size_init;
  std::uint8_t window_size_invalid;
  bool clear_from_valid_to_invalid;
  bool transit_to_invalid_extended;
  bool combined_no_data_init_count;

  SmConfig() noexcept
      : window_size_valid(0u), min_ok_state_init(0u), max_error_state_init(0u),
        min_ok_state_valid(0u), max_error_state_valid(0u), min_ok_state_invalid(0u),
        max_error_state_invalid(0u), window_size_init(0u), window_size_invalid(0u),
        clear_from_valid_to_invalid(false), transit_to_invalid_extended(false),
        combined_no_data_init_count(false) {}
};

Std_ReturnType protect_p08(const P08ConfigWide& config, E2E_P08ProtectStateType& state,
                           MutableByteView data) noexcept;
Std_ReturnType check_p08(const P08ConfigWide& config, E2E_P08CheckStateType& state,
                         ByteView data) noexcept;
Std_ReturnType protect_p08m(const P08mConfigWide& config, E2E_P08mProtectStateType& state,
                            std::uint32_t source_id, Std_MessageTypeType message_type,
                            Std_MessageResultType message_result, MutableByteView data) noexcept;
Std_ReturnType source_check_p08m(const P08mConfigWide& config, E2E_P08mCheckStateType& state,
                                 std::uint32_t expected_source_id,
                                 Std_MessageTypeType message_type,
                                 Std_MessageResultType message_result,
                                 ByteView data) noexcept;
Std_ReturnType sink_check_p08m(const P08mConfigWide& config, E2E_P08mCheckStateType& state,
                               std::uint32_t& received_source_id,
                               Std_MessageTypeType message_type,
                               Std_MessageResultType message_result, ByteView data) noexcept;

Status to_classic_p08_config(const P08ConfigWide& config, E2E_P08ConfigType& out) noexcept;
Status to_classic_p08m_config(const P08mConfigWide& config, E2E_P08mConfigType& out) noexcept;

Std_ReturnType sm_check(E2E_PCheckStatusType profile_status, const SmConfig& config,
                        E2E_SMCheckStateType& state) noexcept;
Std_ReturnType sm_check_init(E2E_SMCheckStateType& state, const SmConfig& config) noexcept;
SmConfig from_classic_sm_config(const E2E_SMConfigType& config) noexcept;

}  // namespace e2e
}  // namespace autoeth

#endif