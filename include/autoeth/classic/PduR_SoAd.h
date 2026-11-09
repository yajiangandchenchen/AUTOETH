/* AUTOETH Classic C compatibility: PduR_SoAd.h
 * PduR upper/lower entry points for the SoAd IF and TP routes. */
#ifndef AUTOETH_CLASSIC_PDUR_SOAD_H
#define AUTOETH_CLASSIC_PDUR_SOAD_H

#include "autoeth/classic/PduR.h"

#ifdef __cplusplus
extern "C" {
#endif

void PduR_SoAdIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
void PduR_SoAdIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result);
Std_ReturnType PduR_SoAdIfTriggerTransmit(PduIdType TxPduId, PduInfoType* PduInfoPtr);
BufReq_ReturnType PduR_SoAdTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);
void PduR_SoAdTpRxIndication(PduIdType id, Std_ReturnType result);
BufReq_ReturnType PduR_SoAdTpStartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr);
BufReq_ReturnType PduR_SoAdTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);
void PduR_SoAdTpTxConfirmation(PduIdType id, Std_ReturnType result);

#ifdef __cplusplus
}
#endif

#endif