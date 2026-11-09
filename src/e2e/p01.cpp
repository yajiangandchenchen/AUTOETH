// AUTOETH E2E Profile 01 - independent implementation.
// R6.1/R6.4: the counter domain (modulo 15) and the Init contract belong to
// this profile alone and are never shared with another profile.
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Crc.h"

namespace {

uint8 NextCounter(uint8 counter) noexcept {
  const uint8 next = static_cast<uint8>((static_cast<uint32>(counter) + 1u) % 15u);
  return next;
}

uint8 ComputeCrc(const uint8* data, uint32 length) noexcept {
  return Crc_CalculateCRC8(data, static_cast<uint32>(length), 0u, TRUE);
}

}  // namespace

extern "C" {

Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* ConfigPtr, E2E_P01ProtectStateType* StatePtr, uint8* DataPtr) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 current = StatePtr->Counter;
  DataPtr[8] = static_cast<uint8>(current);
  DataPtr[9] = 0x00u;
  DataPtr[10] = static_cast<uint8>(ComputeCrc(DataPtr, 8u));
  DataPtr[11] = 0x00u;
  StatePtr->Counter = NextCounter(current);
  return E2E_E_OK;
}

Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->LastValidCounter = 0u;
  StatePtr->MaxDeltaCounter = 0u;
  StatePtr->WaitForFirstData = TRUE;
  StatePtr->NewDataAvailable = TRUE;
  StatePtr->LostData = 0u;
  StatePtr->Status = E2E_P01STATUS_NONEWDATA;
  StatePtr->SyncCounter = 0u;
  StatePtr->NoNewOrRepeatedDataCounter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* ConfigPtr, E2E_P01CheckStateType* StatePtr, const uint8* DataPtr) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 received = static_cast<uint8>(DataPtr[8]);
  const uint8 expectedCrc = static_cast<uint8>(ComputeCrc(DataPtr, 8u));
  if (DataPtr[10] != expectedCrc) {
    StatePtr->Status = E2E_P01STATUS_WRONGCRC;
    return E2E_E_OK;
  }
  if (StatePtr->WaitForFirstData == TRUE) {
    StatePtr->Status = E2E_P01STATUS_INITIAL;
    StatePtr->WaitForFirstData = FALSE;
    StatePtr->LastValidCounter = static_cast<uint8>(received);
    return E2E_E_OK;
  }
  if (static_cast<uint8>(received) == StatePtr->LastValidCounter) {
    StatePtr->Status = E2E_P01STATUS_REPEATED;
    return E2E_E_OK;
  }
  StatePtr->Status = E2E_P01STATUS_OK;
  StatePtr->LastValidCounter = static_cast<uint8>(received);
  StatePtr->NewDataAvailable = TRUE;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status, boolean profileBehavior) {
  (void)profileBehavior;
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P01STATUS_OK || Status == E2E_P01STATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P01STATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P01STATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P01STATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  if (Status == E2E_P01STATUS_SYNC) {
    return E2E_P_WRONGSEQUENCE;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
