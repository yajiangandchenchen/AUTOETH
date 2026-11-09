// AUTOETH E2E state machine. R6.9: VALID/INVALID follow real window counts;
// this implementation never fabricates a VALID state.
#include "autoeth/classic/E2E.h"

extern "C" {

Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType* StatePtr, const E2E_SMConfigType* ConfigPtr) {
  if (StatePtr == NULL_PTR || ConfigPtr == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  if (StatePtr->ProfileStatusWindow == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  for (uint8 index = 0u; index < ConfigPtr->WindowSizeInit; ++index) {
    StatePtr->ProfileStatusWindow[index] = E2E_P_NONEWDATA;
  }
  StatePtr->WindowTopIndex = 0u;
  StatePtr->OkCount = 0u;
  StatePtr->ErrorCount = 0u;
  StatePtr->SMState = E2E_SM_INIT;
  StatePtr->NoDataInitCount = 0u;
  return E2E_E_OK;
}

Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus, const E2E_SMConfigType* ConfigPtr,
                           E2E_SMCheckStateType* StatePtr) {
  if (ConfigPtr == NULL_PTR || StatePtr == NULL_PTR || StatePtr->ProfileStatusWindow == NULL_PTR) {
    return E2E_E_INPUTERR_NULL;
  }
  if (ProfileStatus != E2E_P_OK && ProfileStatus != E2E_P_REPEATED &&
      ProfileStatus != E2E_P_WRONGSEQUENCE && ProfileStatus != E2E_P_ERROR &&
      ProfileStatus != E2E_P_NOTAVAILABLE && ProfileStatus != E2E_P_NONEWDATA) {
    return E2E_E_INPUTERR_WRONG;
  }
  switch (ProfileStatus) {
    case E2E_P_OK:
      ++StatePtr->OkCount;
      break;
    case E2E_P_REPEATED:
    case E2E_P_WRONGSEQUENCE:
    case E2E_P_ERROR:
      ++StatePtr->ErrorCount;
      break;
    default:
      break;
  }
  if (StatePtr->OkCount > ConfigPtr->WindowSizeValid) {
    StatePtr->OkCount = ConfigPtr->WindowSizeValid;
  }
  if (StatePtr->ErrorCount > ConfigPtr->WindowSizeValid) {
    StatePtr->ErrorCount = ConfigPtr->WindowSizeValid;
  }
  switch (StatePtr->SMState) {
    case E2E_SM_INIT:
      if (StatePtr->OkCount >= ConfigPtr->MinOkStateInit &&
          StatePtr->OkCount >= ConfigPtr->MinOkStateValid) {
        StatePtr->SMState = E2E_SM_VALID;
      } else if (StatePtr->ErrorCount >= ConfigPtr->MaxErrorStateInit) {
        StatePtr->SMState = E2E_SM_INVALID;
      }
      break;
    case E2E_SM_VALID:
      if (StatePtr->ErrorCount >= ConfigPtr->MaxErrorStateValid) {
        StatePtr->SMState = E2E_SM_INVALID;
      }
      break;
    case E2E_SM_INVALID:
      if (ConfigPtr->transitToInvalidExtended == static_cast<boolean>(0) ||
          StatePtr->OkCount >= ConfigPtr->MinOkStateInvalid) {
        if (StatePtr->OkCount >= ConfigPtr->MinOkStateInvalid) {
          StatePtr->SMState = E2E_SM_INIT;
        }
      }
      break;
    case E2E_SM_DEINIT:
    case E2E_SM_NODATA:
    default:
      break;
  }
  if (ConfigPtr->ClearToInvalid == static_cast<boolean>(1) &&
      StatePtr->SMState == E2E_SM_VALID &&
      StatePtr->ErrorCount >= ConfigPtr->MaxErrorStateValid) {
    StatePtr->SMState = E2E_SM_INVALID;
  }
  return E2E_E_OK;
}

void E2E_GetVersionInfo(Std_VersionInfoType* VersionInfo) {
  if (VersionInfo != NULL_PTR) {
    VersionInfo->vendorID = 0x0000u;
    VersionInfo->moduleID = 0x00CFu;
    VersionInfo->sw_major_version = 0x04u;
    VersionInfo->sw_minor_version = 0x0Au;
    VersionInfo->sw_patch_version = 0x00u;
  }
}

}  // extern "C"