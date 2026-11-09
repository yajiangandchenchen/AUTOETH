// AUTOETH E2E Native wide-domain P08/P08m and SM entry points.
// R6.6: the wide bit geometry converts to the classic uint32 configuration
// with a checked narrowing that never mutates the output on failure.
#include "autoeth/e2e.hpp"
#include "autoeth/detail/bit_geometry.hpp"

namespace autoeth {
namespace e2e {
namespace e2e_e2e_wide_detail {

bool FitsU32(std::uint64_t value) noexcept {
  return value <= static_cast<std::uint64_t>(0xFFFFFFFFu);
}

bool NullRange(const ByteView& view) noexcept { return view.IsNullRange(); }

bool NullRange(const MutableByteView& view) noexcept { return view.IsNullRange(); }

}  // namespace e2e_e2e_wide_detail

using e2e_e2e_wide_detail::FitsU32;
using e2e_e2e_wide_detail::NullRange;

Std_ReturnType protect_p08(const P08ConfigWide& config, E2E_P08ProtectStateType& state,
                           MutableByteView data) noexcept {
  if (NullRange(data) || data.size < 8u) {
    return E2E_E_INPUTERR_NULL;
  }
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return E2E_E_INPUTERR_WRONG;
  }
  E2E_P08ConfigType classic;
  classic.DataID = config.data_id;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  return E2E_P08Protect(&classic, &state, data.data, static_cast<uint32>(data.size));
}

Std_ReturnType check_p08(const P08ConfigWide& config, E2E_P08CheckStateType& state,
                         ByteView data) noexcept {
  if (NullRange(data)) {
    return E2E_E_INPUTERR_NULL;
  }
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return E2E_E_INPUTERR_WRONG;
  }
  E2E_P08ConfigType classic;
  classic.DataID = config.data_id;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  return E2E_P08Check(&classic, &state, data.data, static_cast<uint32>(data.size));
}

Std_ReturnType protect_p08m(const P08mConfigWide& config, E2E_P08mProtectStateType& state,
                            std::uint32_t source_id, Std_MessageTypeType message_type,
                            Std_MessageResultType message_result, MutableByteView data) noexcept {
  if (NullRange(data)) {
    return E2E_E_INPUTERR_NULL;
  }
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return E2E_E_INPUTERR_WRONG;
  }
  E2E_P08mConfigType classic;
  classic.DataID = config.data_id;
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  return E2E_P08mProtect(&classic, &state, source_id, message_type, message_result, data.data,
                         static_cast<uint32>(data.size));
}

Std_ReturnType source_check_p08m(const P08mConfigWide& config, E2E_P08mCheckStateType& state,
                                 std::uint32_t expected_source_id,
                                 Std_MessageTypeType message_type,
                                 Std_MessageResultType message_result, ByteView data) noexcept {
  if (NullRange(data)) {
    return E2E_E_INPUTERR_NULL;
  }
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return E2E_E_INPUTERR_WRONG;
  }
  E2E_P08mConfigType classic;
  classic.DataID = config.data_id;
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  return E2E_P08mSourceCheck(&classic, &state, expected_source_id, message_type, message_result,
                             data.data, static_cast<uint32>(data.size));
}

Std_ReturnType sink_check_p08m(const P08mConfigWide& config, E2E_P08mCheckStateType& state,
                               std::uint32_t& received_source_id,
                               Std_MessageTypeType message_type,
                               Std_MessageResultType message_result, ByteView data) noexcept {
  if (NullRange(data)) {
    return E2E_E_INPUTERR_NULL;
  }
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return E2E_E_INPUTERR_WRONG;
  }
  E2E_P08mConfigType classic;
  classic.DataID = config.data_id;
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  return E2E_P08mSinkCheck(&classic, &state, &received_source_id, message_type, message_result,
                           data.data, static_cast<uint32>(data.size));
}

Status to_classic_p08_config(const P08ConfigWide& config, E2E_P08ConfigType& out) noexcept {
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return Status::Error(ModuleE2e, UnrepresentableClassicBitLength);
  }
  E2E_P08ConfigType classic;
  classic.DataID = config.data_id;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  out = classic;
  return Status::Success();
}

Status to_classic_p08m_config(const P08mConfigWide& config, E2E_P08mConfigType& out) noexcept {
  if (!FitsU32(config.max_data_length_bits) || !FitsU32(config.min_data_length_bits) ||
      !FitsU32(config.offset_bits)) {
    return Status::Error(ModuleE2e, UnrepresentableClassicBitLength);
  }
  E2E_P08mConfigType classic;
  classic.DataID = config.data_id;
  classic.MinDataLength = autoeth::detail::BitsToBytes(config.min_data_length_bits);
  classic.MaxDataLength = autoeth::detail::BitsToBytes(config.max_data_length_bits);
  classic.MaxDeltaCounter = config.max_delta_counter;
  classic.Offset = autoeth::detail::BitsToBytes(config.offset_bits);
  out = classic;
  return Status::Success();
}

Std_ReturnType sm_check(E2E_PCheckStatusType profile_status, const SmConfig& config,
                        E2E_SMCheckStateType& state) noexcept {
  E2E_SMConfigType classic;
  classic.WindowSizeValid = config.window_size_valid;
  classic.MinOkStateInit = config.min_ok_state_init;
  classic.MaxErrorStateInit = config.max_error_state_init;
  classic.MinOkStateValid = config.min_ok_state_valid;
  classic.MaxErrorStateValid = config.max_error_state_valid;
  classic.MinOkStateInvalid = config.min_ok_state_invalid;
  classic.MaxErrorStateInvalid = config.max_error_state_invalid;
  classic.WindowSizeInit = config.window_size_init;
  classic.WindowSizeInvalid = config.window_size_invalid;
  classic.ClearToInvalid = config.clear_from_valid_to_invalid;
  classic.transitToInvalidExtended = config.transit_to_invalid_extended;
  return E2E_SMCheck(profile_status, &classic, &state);
}

Std_ReturnType sm_check_init(E2E_SMCheckStateType& state, const SmConfig& config) noexcept {
  E2E_SMConfigType classic;
  classic.WindowSizeValid = config.window_size_valid;
  classic.MinOkStateInit = config.min_ok_state_init;
  classic.MaxErrorStateInit = config.max_error_state_init;
  classic.MinOkStateValid = config.min_ok_state_valid;
  classic.MaxErrorStateValid = config.max_error_state_valid;
  classic.MinOkStateInvalid = config.min_ok_state_invalid;
  classic.MaxErrorStateInvalid = config.max_error_state_invalid;
  classic.WindowSizeInit = config.window_size_init;
  classic.WindowSizeInvalid = config.window_size_invalid;
  classic.ClearToInvalid = config.clear_from_valid_to_invalid;
  classic.transitToInvalidExtended = config.transit_to_invalid_extended;
  return E2E_SMCheckInit(&state, &classic);
}

SmConfig from_classic_sm_config(const E2E_SMConfigType& config) noexcept {
  SmConfig native;
  native.window_size_valid = config.WindowSizeValid;
  native.min_ok_state_init = config.MinOkStateInit;
  native.max_error_state_init = config.MaxErrorStateInit;
  native.min_ok_state_valid = config.MinOkStateValid;
  native.max_error_state_valid = config.MaxErrorStateValid;
  native.min_ok_state_invalid = config.MinOkStateInvalid;
  native.max_error_state_invalid = config.MaxErrorStateInvalid;
  native.window_size_init = config.WindowSizeInit;
  native.window_size_invalid = config.WindowSizeInvalid;
  native.clear_from_valid_to_invalid = (config.ClearToInvalid != static_cast<boolean>(0));
  native.transit_to_invalid_extended =
      (config.transitToInvalidExtended != static_cast<boolean>(0));
  native.combined_no_data_init_count = false;
  return native;
}

}  // namespace e2e
}  // namespace autoeth