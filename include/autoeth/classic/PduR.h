/* AUTOETH Classic C compatibility: PduR.h
 * One-to-one routing aggregates. Each path has exactly one destination. */
#ifndef AUTOETH_CLASSIC_PDUR_H
#define AUTOETH_CLASSIC_PDUR_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/ComStack_Types.h"
#include "autoeth/classic/SoAd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16 PduR_RoutingPathGroupIdType;
typedef uint16 PduR_PBConfigIdType;

typedef struct AePduRouteType_tag {
  AePduApi sourceApi;
  PduIdType sourcePdu;
  AePduApi destinationApi;
  PduIdType destinationPdu;
  PduIdType confirmationPdu;
  uint16 metadataBytes;
  uint16 groupRefCount;
  const uint16* groupRefs;
  const AeUpperCallbacks* upper;
} AePduRoute;

typedef struct PduR_PBConfigType_tag {
  PduR_PBConfigIdType configurationId;
  uint32 pathCount;
  const AePduRoute* paths;
  uint32 groupCount;
  const AeRoutingGroup* groups;
} PduR_PBConfigType;

void PduR_Init(const PduR_PBConfigType* ConfigPtr);
void PduR_GetVersionInfo(Std_VersionInfoType* versionInfo);
PduR_PBConfigIdType PduR_GetConfigurationId(void);
void PduR_EnableRouting(PduR_RoutingPathGroupIdType id);
void PduR_DisableRouting(PduR_RoutingPathGroupIdType id, boolean initialize);
Std_ReturnType PduR_AutoEthTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
Std_ReturnType PduR_AutoEthCancelTransmit(PduIdType TxPduId);
Std_ReturnType PduR_AutoEthCancelReceive(PduIdType RxPduId);

#ifdef __cplusplus
}
#endif

#endif