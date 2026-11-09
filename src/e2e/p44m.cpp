// AUTOETH E2E Profile 44m - independent method-profile implementation.
// R6.1/R6.4: Source/Sink metadata handling is local to this profile. The
// counter is a plain uint16 with the source width, never a shared domain.
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

bool IntegrityOk(const uint8* data, uint16 length) noexcept {
  const uint32 expected = ComputeCrc(data, length);
  return static_cast<uint8>(data[10]) == static_cast<uint8>(expected);
}

}  // namespace

extern "C" {

Std_ReturnType E2E_P44mProtectInit(E2E_P44mProtectStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P44mProtect(const E2E_P44mConfigType* ConfigPtr, E2E_P44mProtectStateType* StatePtr, uint32 SourceID,
                       Std_MessageTypeType MessageType, Std_MessageResultType MessageResult,
                       uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  DataPtr[8] = static_cast<uint8>(StatePtr->Counter);
  DataPtr[9] = static_cast<uint8>(MessageType);
  DataPtr[10] = static_cast<uint8>(ComputeCrc(DataPtr, Length));
  DataPtr[11] = static_cast<uint8>(SourceID & 0x1Fu);
  StatePtr->Counter = NextCounter(StatePtr->Counter);
  return E2E_E_OK;
}

Std_ReturnType E2E_P44mCheckInit(E2E_P44mCheckStateType* StatePtr) {
  if (StatePtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  StatePtr->Status = E2E_P44MSTATUS_NONEWDATA;
  StatePtr->Counter = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_P44mSourceCheck(const E2E_P44mConfigType* ConfigPtr, E2E_P44mCheckStateType* StatePtr, uint32 SourceID,
                           Std_MessageTypeType MessageType, Std_MessageResultType MessageResult,
                           const uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  (void)MessageType;
  (void)MessageResult;
  if (!IntegrityOk(DataPtr, Length) || static_cast<uint32>(DataPtr[11] & 0x1Fu) != (SourceID & 0x1Fu)) {
    StatePtr->Status = E2E_P44MSTATUS_ERROR;
    return E2E_E_OK;
  }
  StatePtr->Counter = static_cast<uint16>(DataPtr[8]);
  StatePtr->Status = E2E_P44MSTATUS_OK;
  return E2E_E_OK;
}

Std_ReturnType E2E_P44mSinkCheck(const E2E_P44mConfigType* ConfigPtr, E2E_P44mCheckStateType* StatePtr, uint32* SourceID,
                         Std_MessageTypeType MessageType, Std_MessageResultType MessageResult,
                         const uint8* DataPtr, uint16 Length) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || DataPtr == NULL_PTR ||
      SourceID == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  (void)MessageType;
  (void)MessageResult;
  if (!IntegrityOk(DataPtr, Length)) {
    StatePtr->Status = E2E_P44MSTATUS_ERROR;
    return E2E_E_OK;
  }
  *SourceID = static_cast<uint32>(DataPtr[11]) & 0x0FFFFFFFu;
  StatePtr->Counter = static_cast<uint16>(DataPtr[8]);
  StatePtr->Status = E2E_P44MSTATUS_OK;
  return E2E_E_OK;
}

E2E_PCheckStatusType E2E_P44mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P44mCheckStatusType Status) {
  if (CheckReturn != E2E_E_OK) {
    return E2E_P_ERROR;
  }
  if (Status == E2E_P44MSTATUS_OK || Status == E2E_P44MSTATUS_OKSOMELOST) {
    return E2E_P_OK;
  }
  if (Status == E2E_P44MSTATUS_REPEATED) {
    return E2E_P_REPEATED;
  }
  if (Status == E2E_P44MSTATUS_WRONGSEQUENCE) {
    return E2E_P_WRONGSEQUENCE;
  }
  if (Status == E2E_P44MSTATUS_NONEWDATA) {
    return E2E_P_NONEWDATA;
  }
  return E2E_P_ERROR;
}

}  // extern "C"
