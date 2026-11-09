/* AUTOETH Classic C compatibility: PduR_SomeIpTp.h
 * PduR entry points for the SomeIpTp route. */
#ifndef AUTOETH_CLASSIC_PDUR_SOMEIPTP_H
#define AUTOETH_CLASSIC_PDUR_SOMEIPTP_H

#include "autoeth/classic/PduR.h"

#ifdef __cplusplus
extern "C" {
#endif

Std_ReturnType PduR_SomeIpTpTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
BufReq_ReturnType PduR_SomeIpTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);
void PduR_SomeIpTpRxIndication(PduIdType id, Std_ReturnType result);
BufReq_ReturnType PduR_SomeIpTpStartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr);
BufReq_ReturnType PduR_SomeIpTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);
void PduR_SomeIpTpTxConfirmation(PduIdType id, Std_ReturnType result);

#ifdef __cplusplus
}
#endif

#endif