/* AUTOETH Classic C compatibility: ComStack_Types.h
 * PduInfoType keeps the source field order SduDataPtr, MetaDataPtr, SduLength.
 * BufReq_ReturnType values stay in their own domain (never cast to
 * Std_ReturnType). RetryInfoType keeps TpDataState then TxTpDataCnt. */
#ifndef AUTOETH_CLASSIC_COMSTACK_TYPES_H
#define AUTOETH_CLASSIC_COMSTACK_TYPES_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/ComStack_Cfg.h"

typedef struct PduInfoType_tag {
  uint8* SduDataPtr;
  uint8* MetaDataPtr;
  PduLengthType SduLength;
} PduInfoType;

typedef uint8 BufReq_ReturnType;

#define BUFREQ_OK ((BufReq_ReturnType)0x00u)
#define BUFREQ_E_NOT_OK ((BufReq_ReturnType)0x01u)
#define BUFREQ_E_BUSY ((BufReq_ReturnType)0x02u)
#define BUFREQ_E_OVFL ((BufReq_ReturnType)0x03u)

typedef uint8 TpDataStateType;

#define TP_DATACONF ((TpDataStateType)0x00u)
#define TP_DATARETRY ((TpDataStateType)0x01u)
#define TP_CONFPENDING ((TpDataStateType)0x02u)

typedef struct RetryInfoType_tag {
  TpDataStateType TpDataState;
  PduLengthType TxTpDataCnt;
} RetryInfoType;

typedef uint8 NotifResultType;
#define NTFRSLT_OK ((NotifResultType)0x00u)
#define NTFRSLT_E_NOT_OK ((NotifResultType)0x01u)
#define NTFRSLT_E_TIMEOUT_A ((NotifResultType)0x02u)
#define NTFRSLT_E_TIMEOUT_BS ((NotifResultType)0x03u)
#define NTFRSLT_E_TIMEOUT_CR ((NotifResultType)0x04u)
#define NTFRSLT_E_WRONG_SN ((NotifResultType)0x05u)

#endif