// AUTOETH E2E Profile 05 - independent implementation.
// R6.1/R6.4: the counter domain (native width of uint8) and the Init contract belong to
// this profile alone and are never shared with another profile.
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Crc.h"

namespace {

uint8 NextCounter(uint8 counter) noexcept {
  const uint8 next = static_cast<uint8>(static_cast<uint32>(counter) + 1u);
  return next;
}

uint16 ComputeCrc(const uint8* data, uint16 length) noexcept {
  return Crc_CalculateCRC16(data, static_cast<uint32>(length), 0u, TRUE);
}

}  // namespace

extern "C" {

Std_ReturnType E2E_P05ProtectInit(E2E_P05ProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P05Protect(const E2E_P05ConfigType* ConfigPtr, E2E_P05ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length) {
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

Std_ReturnType E2E_P05CheckInit(E2E_P05CheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Status = E2E_P05STATUS_NONEWDATA;
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P05Check(const E2E_P05ConfigType* ConfigPtr, E2E_P05CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 expectedCrc = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  if (DataPtr[10] != expectedCrc) {
    StatePtr->Status = E2E_P05STATUS_ERROR;
    return E2E_E_OK;
  }
  StatePtr->Counter = static_cast<uint8>(DataPtr[8]);
  StatePtr->Status = E2E_P05STATUS_OK;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P05MapStatusToSM(Std_ReturnType CheckReturn, E2E_P05CheckStatusType Status) {
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P05STATUS_OK || Status == E2E_P05STATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P05STATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P05STATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P05STATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
