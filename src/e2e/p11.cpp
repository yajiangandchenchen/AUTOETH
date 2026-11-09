// AUTOETH E2E Profile 11 - independent implementation.
// R6.1/R6.4: the counter domain (modulo 15) and the Init contract belong to
// this profile alone and are never shared with another profile.
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Crc.h"

namespace e2e_p11_detail {

uint8 NextCounter(uint8 counter) noexcept {
  const uint8 next = static_cast<uint8>((static_cast<uint32>(counter) + 1u) % 15u);
  return next;
}

uint8 ComputeCrc(const uint8* data, uint16 length) noexcept {
  return Crc_CalculateCRC8(data, static_cast<uint32>(length), 0u, TRUE);
}

}  // namespace e2e_p11_detail

using e2e_p11_detail::NextCounter;
using e2e_p11_detail::ComputeCrc;

extern "C" {

Std_ReturnType E2E_P11ProtectInit(E2E_P11ProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P11Protect(const E2E_P11ConfigType* ConfigPtr, E2E_P11ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 current = StatePtr->Counter;
  DataPtr[8] = static_cast<uint8>(current);
  DataPtr[9] = 0x00u;
  DataPtr[10] = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  DataPtr[11] = 0x00u;
  StatePtr->Counter = NextCounter(current);
  return E2E_E_OK;
}

Std_ReturnType E2E_P11CheckInit(E2E_P11CheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Status = E2E_P11STATUS_NONEWDATA;
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P11Check(const E2E_P11ConfigType* ConfigPtr, E2E_P11CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 expectedCrc = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  if (DataPtr[10] != expectedCrc) {
    StatePtr->Status = E2E_P11STATUS_ERROR;
    return E2E_E_OK;
  }
  StatePtr->Counter = static_cast<uint8>(DataPtr[8]);
  StatePtr->Status = E2E_P11STATUS_OK;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P11MapStatusToSM(Std_ReturnType CheckReturn, E2E_P11CheckStatusType Status) {
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P11STATUS_OK || Status == E2E_P11STATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P11STATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P11STATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P11STATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
