// AUTOETH E2E Profile 08 - independent implementation.
// R6.1/R6.4: the counter domain (native width of uint32) and the Init contract belong to
// this profile alone and are never shared with another profile.
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Crc.h"

namespace {

uint32 NextCounter(uint32 counter) noexcept {
  const uint32 next = static_cast<uint32>(static_cast<uint32>(counter) + 1u);
  return next;
}

uint64 ComputeCrc(const uint8* data, uint32 length) noexcept {
  return Crc_CalculateCRC64(data, static_cast<uint32>(length), 0u, TRUE);
}

}  // namespace

extern "C" {

Std_ReturnType E2E_P08ProtectInit(E2E_P08ProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P08Protect(const E2E_P08ConfigType* ConfigPtr, E2E_P08ProtectStateType* StatePtr, uint8* DataPtr, uint32 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint32 current = StatePtr->Counter;
  DataPtr[8] = static_cast<uint8>(current);
  DataPtr[9] = 0x00u;
  DataPtr[10] = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  DataPtr[11] = 0x00u;
  StatePtr->Counter = NextCounter(current);
  return E2E_E_OK;
}

Std_ReturnType E2E_P08CheckInit(E2E_P08CheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Status = E2E_P08STATUS_NONEWDATA;
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P08Check(const E2E_P08ConfigType* ConfigPtr, E2E_P08CheckStateType* StatePtr, const uint8* DataPtr, uint32 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  const uint8 expectedCrc = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  if (DataPtr[10] != expectedCrc) {
    StatePtr->Status = E2E_P08STATUS_ERROR;
    return E2E_E_OK;
  }
  StatePtr->Counter = static_cast<uint32>(DataPtr[8]);
  StatePtr->Status = E2E_P08STATUS_OK;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P08MapStatusToSM(Std_ReturnType CheckReturn, E2E_P08CheckStatusType Status) {
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P08STATUS_OK || Status == E2E_P08STATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P08STATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P08STATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P08STATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
