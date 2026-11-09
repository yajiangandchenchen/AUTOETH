// AUTOETH E2E Profile 44 - independent implementation.
// R6.1/R6.4: the counter domain (native width of uint16) and the Init contract belong to
// this profile alone and are never shared with another profile.
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Crc.h"

namespace {

uint16 NextCounter(uint16 counter) noexcept {
  const uint16 next = static_cast<uint16>(static_cast<uint32>(counter) + 1u);
  return next;
}

uint32 ComputeCrc(const uint8* data, uint16 length) noexcept {
  return Crc_CalculateCRC32P4(data, static_cast<uint32>(length), 0u, TRUE);
}

}  // namespace

extern "C" {

Std_ReturnType E2E_P44ProtectInit(E2E_P44ProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P44Protect(const E2E_P44ConfigType* ConfigPtr, E2E_P44ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint16 current = StatePtr->Counter;
  DataPtr[8] = static_cast<uint8>(current);
  DataPtr[9] = 0x00u;
  DataPtr[10] = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  DataPtr[11] = 0x00u;
  StatePtr->Counter = NextCounter(current);
  return E2E_E_OK;
}

Std_ReturnType E2E_P44CheckInit(E2E_P44CheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Status = E2E_P44STATUS_NONEWDATA;
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P44Check(const E2E_P44ConfigType* ConfigPtr, E2E_P44CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 expectedCrc = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  if (DataPtr[10] != expectedCrc) {
    StatePtr->Status = E2E_P44STATUS_ERROR;
    return E2E_E_OK;
  }
  StatePtr->Counter = static_cast<uint16>(DataPtr[8]);
  StatePtr->Status = E2E_P44STATUS_OK;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P44MapStatusToSM(Std_ReturnType CheckReturn, E2E_P44CheckStatusType Status) {
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P44STATUS_OK || Status == E2E_P44STATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P44STATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P44STATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P44STATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
