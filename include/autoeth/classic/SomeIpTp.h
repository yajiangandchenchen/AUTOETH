/* AUTOETH Classic C compatibility: SomeIpTp.h
 * The C SomeIpTp module is CP_STREAMING; FO_CONTIGUOUS is a separate Native
 * strategy and never shares this state. */
#ifndef AUTOETH_CLASSIC_SOMEIPTP_H
#define AUTOETH_CLASSIC_SOMEIPTP_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/ComStack_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AeTpTxPduType_tag {
  PduIdType sourcePdu;
  PduIdType lowerPdu;
  PduIdType confirmationPdu;
  uint32 maxSegmentPayloadBytes;
  uint16 burstCount;
  uint64 minimumSeparationNs;
} AeTpTxPdu;

typedef struct AeTpRxPduType_tag {
  PduIdType lowerPdu;
  PduIdType upperPdu;
  uint32 maxOriginalBytes;
  uint64 reassemblyTimeoutNs;
} AeTpRxPdu;

typedef struct SomeIpTp_ConfigType_tag {
  uint32 txPduCount;
  const AeTpTxPdu* txPdus;
  uint32 rxPduCount;
  const AeTpRxPdu* rxPdus;
  uint64 mainPeriodNs;
} SomeIpTp_ConfigType;

void SomeIpTp_Init(const SomeIpTp_ConfigType* config);
void SomeIpTp_GetVersionInfo(Std_VersionInfoType* VersionInfo);
Std_ReturnType SomeIpTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
void SomeIpTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
void SomeIpTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);
void SomeIpTp_MainFunctionRx(void);
void SomeIpTp_MainFunctionTx(void);

#ifdef __cplusplus
}
#endif

#endif