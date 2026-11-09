/* AUTOETH Classic C compatibility: E2E.h
 * Original names, ordered parameters and approved host adaptations only
 * (04m Sink state pointer, 07 Map first argument, 08m Sink SourceID pointer).
 * The sixteen "kind enumeration" status types are uint8 typedefs here. */
#ifndef AUTOETH_CLASSIC_E2E_H
#define AUTOETH_CLASSIC_E2E_H

#include "autoeth/classic/Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define E2E_E_OK ((Std_ReturnType)0x00u)
#define E2E_E_INPUTERR_NULL ((Std_ReturnType)0x13u)
#define E2E_E_INPUTERR_WRONG ((Std_ReturnType)0x17u)
#define E2E_E_INTERR ((Std_ReturnType)0x19u)

typedef uint8 E2E_P01DataIDMode;
#define E2E_P01_DATAID_BOTH ((E2E_P01DataIDMode)0u)
#define E2E_P01_DATAID_ALT ((E2E_P01DataIDMode)1u)
#define E2E_P01_DATAID_LOW ((E2E_P01DataIDMode)2u)
#define E2E_P01_DATAID_NIBBLE ((E2E_P01DataIDMode)3u)

typedef uint8 E2E_P01CheckStatusType;
#define E2E_P01STATUS_OK ((E2E_P01CheckStatusType)0x00u)
#define E2E_P01STATUS_NONEWDATA ((E2E_P01CheckStatusType)0x01u)
#define E2E_P01STATUS_WRONGCRC ((E2E_P01CheckStatusType)0x02u)
#define E2E_P01STATUS_SYNC ((E2E_P01CheckStatusType)0x03u)
#define E2E_P01STATUS_INITIAL ((E2E_P01CheckStatusType)0x04u)
#define E2E_P01STATUS_REPEATED ((E2E_P01CheckStatusType)0x08u)
#define E2E_P01STATUS_OKSOMELOST ((E2E_P01CheckStatusType)0x20u)
#define E2E_P01STATUS_WRONGSEQUENCE ((E2E_P01CheckStatusType)0x40u)

typedef uint8 E2E_P02CheckStatusType;
#define E2E_P02STATUS_OK ((E2E_P02CheckStatusType)0x00u)
#define E2E_P02STATUS_NONEWDATA ((E2E_P02CheckStatusType)0x01u)
#define E2E_P02STATUS_WRONGCRC ((E2E_P02CheckStatusType)0x02u)
#define E2E_P02STATUS_SYNC ((E2E_P02CheckStatusType)0x03u)
#define E2E_P02STATUS_INITIAL ((E2E_P02CheckStatusType)0x04u)
#define E2E_P02STATUS_REPEATED ((E2E_P02CheckStatusType)0x08u)
#define E2E_P02STATUS_OKSOMELOST ((E2E_P02CheckStatusType)0x20u)
#define E2E_P02STATUS_WRONGSEQUENCE ((E2E_P02CheckStatusType)0x40u)

typedef uint8 E2E_P04CheckStatusType;
#define E2E_P04STATUS_OK ((E2E_P04CheckStatusType)0x00u)
#define E2E_P04STATUS_NONEWDATA ((E2E_P04CheckStatusType)0x01u)
#define E2E_P04STATUS_ERROR ((E2E_P04CheckStatusType)0x07u)
#define E2E_P04STATUS_REPEATED ((E2E_P04CheckStatusType)0x08u)
#define E2E_P04STATUS_OKSOMELOST ((E2E_P04CheckStatusType)0x20u)
#define E2E_P04STATUS_WRONGSEQUENCE ((E2E_P04CheckStatusType)0x40u)

typedef uint8 E2E_P04mCheckStatusType;
#define E2E_P04MSTATUS_OK ((E2E_P04mCheckStatusType)0x00u)
#define E2E_P04MSTATUS_NONEWDATA ((E2E_P04mCheckStatusType)0x01u)
#define E2E_P04MSTATUS_ERROR ((E2E_P04mCheckStatusType)0x07u)
#define E2E_P04MSTATUS_REPEATED ((E2E_P04mCheckStatusType)0x08u)
#define E2E_P04MSTATUS_OKSOMELOST ((E2E_P04mCheckStatusType)0x20u)
#define E2E_P04MSTATUS_WRONGSEQUENCE ((E2E_P04mCheckStatusType)0x40u)

typedef uint8 E2E_P05CheckStatusType;
#define E2E_P05STATUS_OK ((E2E_P05CheckStatusType)0x00u)
#define E2E_P05STATUS_NONEWDATA ((E2E_P05CheckStatusType)0x01u)
#define E2E_P05STATUS_ERROR ((E2E_P05CheckStatusType)0x07u)
#define E2E_P05STATUS_REPEATED ((E2E_P05CheckStatusType)0x08u)
#define E2E_P05STATUS_OKSOMELOST ((E2E_P05CheckStatusType)0x20u)
#define E2E_P05STATUS_WRONGSEQUENCE ((E2E_P05CheckStatusType)0x40u)

typedef uint8 E2E_P06CheckStatusType;
#define E2E_P06STATUS_OK ((E2E_P06CheckStatusType)0x00u)
#define E2E_P06STATUS_NONEWDATA ((E2E_P06CheckStatusType)0x01u)
#define E2E_P06STATUS_ERROR ((E2E_P06CheckStatusType)0x07u)
#define E2E_P06STATUS_REPEATED ((E2E_P06CheckStatusType)0x08u)
#define E2E_P06STATUS_OKSOMELOST ((E2E_P06CheckStatusType)0x20u)
#define E2E_P06STATUS_WRONGSEQUENCE ((E2E_P06CheckStatusType)0x40u)

typedef uint8 E2E_P07CheckStatusType;
#define E2E_P07STATUS_OK ((E2E_P07CheckStatusType)0x00u)
#define E2E_P07STATUS_NONEWDATA ((E2E_P07CheckStatusType)0x01u)
#define E2E_P07STATUS_ERROR ((E2E_P07CheckStatusType)0x07u)
#define E2E_P07STATUS_REPEATED ((E2E_P07CheckStatusType)0x08u)
#define E2E_P07STATUS_OKSOMELOST ((E2E_P07CheckStatusType)0x20u)
#define E2E_P07STATUS_WRONGSEQUENCE ((E2E_P07CheckStatusType)0x40u)

typedef uint8 E2E_P07mCheckStatusType;
#define E2E_P07MSTATUS_OK ((E2E_P07mCheckStatusType)0x00u)
#define E2E_P07MSTATUS_NONEWDATA ((E2E_P07mCheckStatusType)0x01u)
#define E2E_P07MSTATUS_ERROR ((E2E_P07mCheckStatusType)0x07u)
#define E2E_P07MSTATUS_REPEATED ((E2E_P07mCheckStatusType)0x08u)
#define E2E_P07MSTATUS_OKSOMELOST ((E2E_P07mCheckStatusType)0x20u)
#define E2E_P07MSTATUS_WRONGSEQUENCE ((E2E_P07mCheckStatusType)0x40u)

typedef uint8 E2E_P08CheckStatusType;
#define E2E_P08STATUS_OK ((E2E_P08CheckStatusType)0x00u)
#define E2E_P08STATUS_NONEWDATA ((E2E_P08CheckStatusType)0x01u)
#define E2E_P08STATUS_ERROR ((E2E_P08CheckStatusType)0x07u)
#define E2E_P08STATUS_REPEATED ((E2E_P08CheckStatusType)0x08u)
#define E2E_P08STATUS_OKSOMELOST ((E2E_P08CheckStatusType)0x20u)
#define E2E_P08STATUS_WRONGSEQUENCE ((E2E_P08CheckStatusType)0x40u)

typedef uint8 E2E_P08mCheckStatusType;
#define E2E_P08MSTATUS_OK ((E2E_P08mCheckStatusType)0x00u)
#define E2E_P08MSTATUS_NONEWDATA ((E2E_P08mCheckStatusType)0x01u)
#define E2E_P08MSTATUS_ERROR ((E2E_P08mCheckStatusType)0x07u)
#define E2E_P08MSTATUS_REPEATED ((E2E_P08mCheckStatusType)0x08u)
#define E2E_P08MSTATUS_OKSOMELOST ((E2E_P08mCheckStatusType)0x20u)
#define E2E_P08MSTATUS_WRONGSEQUENCE ((E2E_P08mCheckStatusType)0x40u)

typedef uint8 E2E_P11DataIDMode;
#define E2E_P11_DATAID_BOTH ((E2E_P11DataIDMode)0u)
#define E2E_P11_DATAID_NIBBLE ((E2E_P11DataIDMode)3u)

typedef uint8 E2E_P11CheckStatusType;
#define E2E_P11STATUS_OK ((E2E_P11CheckStatusType)0x00u)
#define E2E_P11STATUS_NONEWDATA ((E2E_P11CheckStatusType)0x01u)
#define E2E_P11STATUS_ERROR ((E2E_P11CheckStatusType)0x07u)
#define E2E_P11STATUS_REPEATED ((E2E_P11CheckStatusType)0x08u)
#define E2E_P11STATUS_OKSOMELOST ((E2E_P11CheckStatusType)0x20u)
#define E2E_P11STATUS_WRONGSEQUENCE ((E2E_P11CheckStatusType)0x40u)

typedef uint8 E2E_P22CheckStatusType;
#define E2E_P22STATUS_OK ((E2E_P22CheckStatusType)0x00u)
#define E2E_P22STATUS_NONEWDATA ((E2E_P22CheckStatusType)0x01u)
#define E2E_P22STATUS_ERROR ((E2E_P22CheckStatusType)0x07u)
#define E2E_P22STATUS_REPEATED ((E2E_P22CheckStatusType)0x08u)
#define E2E_P22STATUS_OKSOMELOST ((E2E_P22CheckStatusType)0x20u)
#define E2E_P22STATUS_WRONGSEQUENCE ((E2E_P22CheckStatusType)0x40u)

typedef uint8 E2E_P44CheckStatusType;
#define E2E_P44STATUS_OK ((E2E_P44CheckStatusType)0x00u)
#define E2E_P44STATUS_NONEWDATA ((E2E_P44CheckStatusType)0x01u)
#define E2E_P44STATUS_ERROR ((E2E_P44CheckStatusType)0x07u)
#define E2E_P44STATUS_REPEATED ((E2E_P44CheckStatusType)0x08u)
#define E2E_P44STATUS_OKSOMELOST ((E2E_P44CheckStatusType)0x20u)
#define E2E_P44STATUS_WRONGSEQUENCE ((E2E_P44CheckStatusType)0x40u)

typedef uint8 E2E_P44mCheckStatusType;
#define E2E_P44MSTATUS_OK ((E2E_P44mCheckStatusType)0x00u)
#define E2E_P44MSTATUS_NONEWDATA ((E2E_P44mCheckStatusType)0x01u)
#define E2E_P44MSTATUS_ERROR ((E2E_P44mCheckStatusType)0x07u)
#define E2E_P44MSTATUS_REPEATED ((E2E_P44mCheckStatusType)0x08u)
#define E2E_P44MSTATUS_OKSOMELOST ((E2E_P44mCheckStatusType)0x20u)
#define E2E_P44MSTATUS_WRONGSEQUENCE ((E2E_P44mCheckStatusType)0x40u)

typedef uint8 E2E_PCheckStatusType;
#define E2E_P_OK ((E2E_PCheckStatusType)0x00u)
#define E2E_P_REPEATED ((E2E_PCheckStatusType)0x01u)
#define E2E_P_WRONGSEQUENCE ((E2E_PCheckStatusType)0x02u)
#define E2E_P_ERROR ((E2E_PCheckStatusType)0x03u)
#define E2E_P_NOTAVAILABLE ((E2E_PCheckStatusType)0x04u)
#define E2E_P_NONEWDATA ((E2E_PCheckStatusType)0x05u)

typedef uint8 E2E_SMStateType;
#define E2E_SM_VALID ((E2E_SMStateType)0x00u)
#define E2E_SM_DEINIT ((E2E_SMStateType)0x01u)
#define E2E_SM_NODATA ((E2E_SMStateType)0x02u)
#define E2E_SM_INIT ((E2E_SMStateType)0x03u)
#define E2E_SM_INVALID ((E2E_SMStateType)0x04u)

typedef struct E2E_P01ConfigType_tag {
  uint16 CounterOffset;
  uint16 CRCOffset;
  uint16 DataID;
  uint16 DataIDNibbleOffset;
  E2E_P01DataIDMode DataIDMode;
  uint16 DataLength;
  uint8 MaxDeltaCounterInit;
  uint8 MaxNoNewOrRepeatedData;
  uint8 SyncCounterInit;
} E2E_P01ConfigType;

typedef struct E2E_P01ProtectStateType_tag {
  uint8 Counter;
} E2E_P01ProtectStateType;

typedef struct E2E_P01CheckStateType_tag {
  uint8 LastValidCounter;
  uint8 MaxDeltaCounter;
  boolean WaitForFirstData;
  boolean NewDataAvailable;
  uint8 LostData;
  E2E_P01CheckStatusType Status;
  uint8 SyncCounter;
  uint8 NoNewOrRepeatedDataCounter;
} E2E_P01CheckStateType;

typedef struct E2E_P02ConfigType_tag {
  uint16 DataLength;
  uint8 DataIDList[16];
  uint8 MaxDeltaCounterInit;
  uint8 MaxNoNewOrRepeatedData;
  uint8 SyncCounterInit;
  uint16 Offset;
} E2E_P02ConfigType;

typedef struct E2E_P02ProtectStateType_tag {
  uint8 Counter;
} E2E_P02ProtectStateType;

typedef struct E2E_P02CheckStateType_tag {
  uint8 LastValidCounter;
  uint8 MaxDeltaCounter;
  boolean WaitForFirstData;
  boolean NewDataAvailable;
  uint8 LostData;
  E2E_P02CheckStatusType Status;
  uint8 SyncCounter;
  uint8 NoNewOrRepeatedDataCounter;
} E2E_P02CheckStateType;

typedef struct E2E_P04ConfigType_tag {
  uint32 DataID;
  uint16 Offset;
  uint16 MinDataLength;
  uint16 MaxDataLength;
  uint16 MaxDeltaCounter;
} E2E_P04ConfigType;

typedef struct E2E_P04ProtectStateType_tag {
  uint16 Counter;
} E2E_P04ProtectStateType;

typedef struct E2E_P04CheckStateType_tag {
  E2E_P04CheckStatusType Status;
  uint16 Counter;
} E2E_P04CheckStateType;

typedef struct E2E_P04mConfigType_tag {
  uint32 DataID;
  uint16 Offset;
  uint16 MinDataLength;
  uint16 MaxDataLength;
  uint16 MaxDeltaCounter;
} E2E_P04mConfigType;

typedef struct E2E_P04mProtectStateType_tag {
  uint16 Counter;
} E2E_P04mProtectStateType;

typedef struct E2E_P04mCheckStateType_tag {
  E2E_P04mCheckStatusType Status;
  uint16 Counter;
} E2E_P04mCheckStateType;

typedef struct E2E_P05ConfigType_tag {
  uint16 Offset;
  uint16 DataLength;
  uint16 DataID;
  uint8 MaxDeltaCounter;
} E2E_P05ConfigType;

typedef struct E2E_P05ProtectStateType_tag {
  uint8 Counter;
} E2E_P05ProtectStateType;

typedef struct E2E_P05CheckStateType_tag {
  E2E_P05CheckStatusType Status;
  uint8 Counter;
} E2E_P05CheckStateType;

typedef struct E2E_P06ConfigType_tag {
  uint16 Offset;
  uint16 MinDataLength;
  uint16 MaxDataLength;
  uint16 DataID;
  uint8 MaxDeltaCounter;
} E2E_P06ConfigType;

typedef struct E2E_P06ProtectStateType_tag {
  uint8 Counter;
} E2E_P06ProtectStateType;

typedef struct E2E_P06CheckStateType_tag {
  E2E_P06CheckStatusType Status;
  uint8 Counter;
} E2E_P06CheckStateType;

typedef struct E2E_P07ConfigType_tag {
  uint32 DataID;
  uint32 Offset;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint32 MaxDeltaCounter;
} E2E_P07ConfigType;

typedef struct E2E_P07ProtectStateType_tag {
  uint32 Counter;
} E2E_P07ProtectStateType;

typedef struct E2E_P07CheckStateType_tag {
  E2E_P07CheckStatusType Status;
  uint32 Counter;
} E2E_P07CheckStateType;

typedef struct E2E_P07mConfigType_tag {
  uint32 DataID;
  uint32 Offset;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint32 MaxDeltaCounter;
} E2E_P07mConfigType;

typedef struct E2E_P07mProtectStateType_tag {
  uint32 Counter;
} E2E_P07mProtectStateType;

typedef struct E2E_P07mCheckStateType_tag {
  E2E_P07mCheckStatusType Status;
  uint32 Counter;
} E2E_P07mCheckStateType;

typedef struct E2E_P08ConfigType_tag {
  uint32 DataID;
  uint32 Offset;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint32 MaxDeltaCounter;
} E2E_P08ConfigType;

typedef struct E2E_P08ProtectStateType_tag {
  uint32 Counter;
} E2E_P08ProtectStateType;

typedef struct E2E_P08CheckStateType_tag {
  E2E_P08CheckStatusType Status;
  uint32 Counter;
} E2E_P08CheckStateType;

typedef struct E2E_P08mConfigType_tag {
  uint32 DataID;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint32 MaxDeltaCounter;
  uint32 Offset;
} E2E_P08mConfigType;

typedef struct E2E_P08mProtectStateType_tag {
  uint32 Counter;
} E2E_P08mProtectStateType;

typedef struct E2E_P08mCheckStateType_tag {
  E2E_P08mCheckStatusType Status;
  uint32 Counter;
} E2E_P08mCheckStateType;

typedef struct E2E_P11ConfigType_tag {
  uint16 DataLength;
  uint16 DataID;
  uint8 MaxDeltaCounter;
  E2E_P11DataIDMode DataIDMode;
  uint16 CRCOffset;
  uint16 CounterOffset;
  uint16 DataIDNibbleOffset;
} E2E_P11ConfigType;

typedef struct E2E_P11ProtectStateType_tag {
  uint8 Counter;
} E2E_P11ProtectStateType;

typedef struct E2E_P11CheckStateType_tag {
  E2E_P11CheckStatusType Status;
  uint8 Counter;
} E2E_P11CheckStateType;

typedef struct E2E_P22ConfigType_tag {
  uint16 MinDataLength;
  uint16 MaxDataLength;
  uint8 DataIDList[16];
  uint8 MaxDeltaCounter;
  uint16 Offset;
} E2E_P22ConfigType;

typedef struct E2E_P22ProtectStateType_tag {
  uint8 Counter;
} E2E_P22ProtectStateType;

typedef struct E2E_P22CheckStateType_tag {
  uint8 Counter;
  E2E_P22CheckStatusType Status;
} E2E_P22CheckStateType;

typedef struct E2E_P44ConfigType_tag {
  uint32 DataID;
  uint32 Offset;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint16 MaxDeltaCounter;
} E2E_P44ConfigType;

typedef struct E2E_P44ProtectStateType_tag {
  uint16 Counter;
} E2E_P44ProtectStateType;

typedef struct E2E_P44CheckStateType_tag {
  E2E_P44CheckStatusType Status;
  uint16 Counter;
} E2E_P44CheckStateType;

typedef struct E2E_P44mConfigType_tag {
  uint32 DataID;
  uint32 MinDataLength;
  uint32 MaxDataLength;
  uint16 MaxDeltaCounter;
  uint32 Offset;
} E2E_P44mConfigType;

typedef struct E2E_P44mProtectStateType_tag {
  uint16 Counter;
} E2E_P44mProtectStateType;

typedef struct E2E_P44mCheckStateType_tag {
  E2E_P44mCheckStatusType Status;
  uint16 Counter;
} E2E_P44mCheckStateType;

typedef struct E2E_SMConfigType_tag {
  uint8 WindowSizeValid;
  uint8 MinOkStateInit;
  uint8 MaxErrorStateInit;
  uint8 MinOkStateValid;
  uint8 MaxErrorStateValid;
  uint8 MinOkStateInvalid;
  uint8 MaxErrorStateInvalid;
  uint8 WindowSizeInit;
  uint8 WindowSizeInvalid;
  boolean ClearToInvalid;
  boolean transitToInvalidExtended;
} E2E_SMConfigType;

typedef struct E2E_SMCheckStateType_tag {
  uint8* ProfileStatusWindow;
  uint8 WindowTopIndex;
  uint8 OkCount;
  uint8 ErrorCount;
  E2E_SMStateType SMState;
  uint8 NoDataInitCount;
} E2E_SMCheckStateType;

Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* ConfigPtr, E2E_P01ProtectStateType* StatePtr, uint8* DataPtr);
Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* StatePtr);
Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* Config, E2E_P01CheckStateType* State, const uint8* Data);
Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status, boolean profileBehavior);

Std_ReturnType E2E_P02Protect(const E2E_P02ConfigType* ConfigPtr, E2E_P02ProtectStateType* StatePtr, uint8* DataPtr);
Std_ReturnType E2E_P02ProtectInit(E2E_P02ProtectStateType* StatePtr);
Std_ReturnType E2E_P02Check(const E2E_P02ConfigType* ConfigPtr, E2E_P02CheckStateType* StatePtr, const uint8* DataPtr);
Std_ReturnType E2E_P02CheckInit(E2E_P02CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P02MapStatusToSM(Std_ReturnType CheckReturn, E2E_P02CheckStatusType Status, boolean profileBehavior);

Std_ReturnType E2E_P04Protect(const E2E_P04ConfigType* ConfigPtr, E2E_P04ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P04ProtectInit(E2E_P04ProtectStateType* StatePtr);
Std_ReturnType E2E_P04Check(const E2E_P04ConfigType* ConfigPtr, E2E_P04CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P04CheckInit(E2E_P04CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P04MapStatusToSM(Std_ReturnType CheckReturn, E2E_P04CheckStatusType Status);

Std_ReturnType E2E_P04mProtect(const E2E_P04mConfigType* ConfigPtr, E2E_P04mProtectStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P04mProtectInit(E2E_P04mProtectStateType* StatePtr);
Std_ReturnType E2E_P04mSourceCheck(const E2E_P04mConfigType* ConfigPtr, E2E_P04mCheckStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P04mSinkCheck(const E2E_P04mConfigType* ConfigPtr, E2E_P04mCheckStateType* StatePtr, uint32* SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P04mCheckInit(E2E_P04mCheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P04mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P04mCheckStatusType Status);

Std_ReturnType E2E_P05Protect(const E2E_P05ConfigType* ConfigPtr, E2E_P05ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P05ProtectInit(E2E_P05ProtectStateType* StatePtr);
Std_ReturnType E2E_P05Check(const E2E_P05ConfigType* ConfigPtr, E2E_P05CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P05CheckInit(E2E_P05CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P05MapStatusToSM(Std_ReturnType CheckReturn, E2E_P05CheckStatusType Status);

Std_ReturnType E2E_P06Protect(const E2E_P06ConfigType* ConfigPtr, E2E_P06ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P06ProtectInit(E2E_P06ProtectStateType* StatePtr);
Std_ReturnType E2E_P06Check(const E2E_P06ConfigType* ConfigPtr, E2E_P06CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P06CheckInit(E2E_P06CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P06MapStatusToSM(Std_ReturnType CheckReturn, E2E_P06CheckStatusType Status);

Std_ReturnType E2E_P07Protect(const E2E_P07ConfigType* ConfigPtr, E2E_P07ProtectStateType* StatePtr, uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P07ProtectInit(E2E_P07ProtectStateType* StatePtr);
Std_ReturnType E2E_P07Check(const E2E_P07ConfigType* ConfigPtr, E2E_P07CheckStateType* StatePtr, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P07CheckInit(E2E_P07CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P07MapStatusToSM(Std_ReturnType CheckReturn, E2E_P07CheckStatusType Status);

Std_ReturnType E2E_P07mProtect(const E2E_P07mConfigType* ConfigPtr, E2E_P07mProtectStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P07mProtectInit(E2E_P07mProtectStateType* StatePtr);
Std_ReturnType E2E_P07mSourceCheck(const E2E_P07mConfigType* ConfigPtr, E2E_P07mCheckStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P07mSinkCheck(const E2E_P07mConfigType* ConfigPtr, E2E_P07mCheckStateType* StatePtr, uint32* SourceID, Std_MessageTypeType MessageType, Std_MessageResultType MessageResult, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P07mCheckInit(E2E_P07mCheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P07mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P07mCheckStatusType Status);

Std_ReturnType E2E_P08Protect(const E2E_P08ConfigType* ConfigPtr, E2E_P08ProtectStateType* StatePtr, uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P08ProtectInit(E2E_P08ProtectStateType* StatePtr);
Std_ReturnType E2E_P08Check(const E2E_P08ConfigType* ConfigPtr, E2E_P08CheckStateType* StatePtr, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P08CheckInit(E2E_P08CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P08MapStatusToSM(Std_ReturnType CheckReturn, E2E_P08CheckStatusType Status);

Std_ReturnType E2E_P08mProtect(const E2E_P08mConfigType* ConfigPtr, E2E_P08mProtectStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P08mProtectInit(E2E_P08mProtectStateType* StatePtr);
Std_ReturnType E2E_P08mSourceCheck(const E2E_P08mConfigType* ConfigPtr, E2E_P08mCheckStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P08mSinkCheck(const E2E_P08mConfigType* ConfigPtr, E2E_P08mCheckStateType* StatePtr, uint32* SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, const uint8* DataPtr, uint32 Length);
Std_ReturnType E2E_P08mCheckInit(E2E_P08mCheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P08mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P08mCheckStatusType Status);

Std_ReturnType E2E_P11Protect(const E2E_P11ConfigType* ConfigPtr, E2E_P11ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P11ProtectInit(E2E_P11ProtectStateType* StatePtr);
Std_ReturnType E2E_P11Check(const E2E_P11ConfigType* ConfigPtr, E2E_P11CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P11CheckInit(E2E_P11CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P11MapStatusToSM(Std_ReturnType CheckReturn, E2E_P11CheckStatusType Status);

Std_ReturnType E2E_P22Protect(const E2E_P22ConfigType* ConfigPtr, E2E_P22ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P22ProtectInit(E2E_P22ProtectStateType* StatePtr);
Std_ReturnType E2E_P22Check(const E2E_P22ConfigType* ConfigPtr, E2E_P22CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P22CheckInit(E2E_P22CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P22MapStatusToSM(Std_ReturnType CheckReturn, E2E_P22CheckStatusType Status);

Std_ReturnType E2E_P44Protect(const E2E_P44ConfigType* ConfigPtr, E2E_P44ProtectStateType* StatePtr, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P44ProtectInit(E2E_P44ProtectStateType* StatePtr);
Std_ReturnType E2E_P44Check(const E2E_P44ConfigType* ConfigPtr, E2E_P44CheckStateType* StatePtr, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P44CheckInit(E2E_P44CheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P44MapStatusToSM(Std_ReturnType CheckReturn, E2E_P44CheckStatusType Status);

Std_ReturnType E2E_P44mProtect(const E2E_P44mConfigType* ConfigPtr, E2E_P44mProtectStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P44mProtectInit(E2E_P44mProtectStateType* StatePtr);
Std_ReturnType E2E_P44mSourceCheck(const E2E_P44mConfigType* ConfigPtr, E2E_P44mCheckStateType* StatePtr, uint32 SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P44mSinkCheck(const E2E_P44mConfigType* ConfigPtr, E2E_P44mCheckStateType* StatePtr, uint32* SourceID, Std_MessageTypeType MessageType, Std_MessageResultType Messageresult, const uint8* DataPtr, uint16 Length);
Std_ReturnType E2E_P44mCheckInit(E2E_P44mCheckStateType* StatePtr);
E2E_PCheckStatusType E2E_P44mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P44mCheckStatusType Status);

Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus, const E2E_SMConfigType* ConfigPtr, E2E_SMCheckStateType* StatePtr);
Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType* StatePtr, const E2E_SMConfigType* ConfigPtr);
void E2E_GetVersionInfo(Std_VersionInfoType* VersionInfo);

#ifdef __cplusplus
}
#endif

#endif