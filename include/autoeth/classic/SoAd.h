/* AUTOETH Classic C compatibility: SoAd.h
 * Ordered host aggregates and route tables (04_Classic_C, host binding).
 * Selected C routes require pduHeaderEnabled=TRUE and TCP TP immediate=TRUE. */
#ifndef AUTOETH_CLASSIC_SOAD_H
#define AUTOETH_CLASSIC_SOAD_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/ComStack_Types.h"
#include "autoeth/classic/TcpIp_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16 SoAd_SoConIdType;
typedef uint16 SoAd_RoutingGroupIdType;

typedef enum {
  SOAD_SOCON_ONLINE = 0,
  SOAD_SOCON_RECONNECT = 1,
  SOAD_SOCON_OFFLINE = 2
} SoAd_SoConModeType;

typedef uint8 AeUpperType;
#define AE_UPPER_IF ((AeUpperType)0u)
#define AE_UPPER_TP ((AeUpperType)1u)

typedef enum {
  AE_PDU_API_AUTOETH = 0,
  AE_PDU_API_SOMEIPTP = 1,
  AE_PDU_API_SOAD_IF = 2,
  AE_PDU_API_SOAD_TP = 3
} AePduApi;

typedef struct AeUpperCallbacksType_tag {
  void (*IfRxIndication)(PduIdType, const PduInfoType*);
  void (*IfTxConfirmation)(PduIdType, Std_ReturnType);
  Std_ReturnType (*TriggerTransmit)(PduIdType, PduInfoType*);
  BufReq_ReturnType (*StartOfReception)(PduIdType, const PduInfoType*, PduLengthType, PduLengthType*);
  BufReq_ReturnType (*CopyRxData)(PduIdType, const PduInfoType*, PduLengthType*);
  void (*TpRxIndication)(PduIdType, Std_ReturnType);
  BufReq_ReturnType (*CopyTxData)(PduIdType, const PduInfoType*, const RetryInfoType*, PduLengthType*);
  void (*TpTxConfirmation)(PduIdType, Std_ReturnType);
  void (*SoConModeChg)(SoAd_SoConIdType, SoAd_SoConModeType);
  void (*LocalIpAddrAssignmentChg)(SoAd_SoConIdType, TcpIp_IpAddrStateType);
} AeUpperCallbacks;

typedef struct AeSoConConfigType_tag {
  SoAd_SoConIdType id;
  uint16 group;
  uint32 nativeEndpointIndex;
  boolean automaticSetup;
  boolean acceptanceFilter;
  boolean pduHeaderEnabled;
  boolean tcpImmediateTpConfirmation;
  TcpIp_SockAddrType configuredRemote;
  boolean remoteIsSet;
  boolean remoteIpWildcard;
  boolean remotePortWildcard;
} AeSoConConfig;

typedef struct AeSoAdTxRouteType_tag {
  PduIdType sourcePdu;
  SoAd_SoConIdType destinationSoCon;
  uint32 headerId;
  PduIdType upperConfirmationPdu;
  AeUpperType upperType;
  boolean triggerTransmit;
  uint16 metadataBytes;
  const AeUpperCallbacks* callbacks;
  uint16 groupRefCount;
  const uint16* groupRefs;
} AeSoAdTxRoute;

typedef struct AeSoAdRxRouteType_tag {
  SoAd_SoConIdType sourceSoCon;
  uint32 headerId;
  PduIdType destinationPdu;
  AeUpperType upperType;
  uint16 metadataBytes;
  const AeUpperCallbacks* callbacks;
  uint16 groupRefCount;
  const uint16* groupRefs;
} AeSoAdRxRoute;

typedef struct AeRoutingGroupType_tag {
  uint16 id;
  boolean initialEnabled;
  boolean txTriggerable;
} AeRoutingGroup;

typedef struct SoAd_ConfigType_tag {
  uint16 schemaVersion;
  uint32 soConCount;
  const AeSoConConfig* soCons;
  uint32 txRouteCount;
  const AeSoAdTxRoute* txRoutes;
  uint32 rxRouteCount;
  const AeSoAdRxRoute* rxRoutes;
  uint32 routingGroupCount;
  const AeRoutingGroup* routingGroups;
} SoAd_ConfigType;

void SoAd_Init(const SoAd_ConfigType* SoAdConfigPtr);
void SoAd_GetVersionInfo(Std_VersionInfoType* versioninfo);
Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
Std_ReturnType SoAd_TpCancelTransmit(PduIdType TxPduId);
Std_ReturnType SoAd_TpCancelReceive(PduIdType RxPduId);
Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType* SoConIdPtr);
Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId);
Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort);
void SoAd_GetSoConMode(SoAd_SoConIdType SoConId, SoAd_SoConModeType* ModePtr);
Std_ReturnType SoAd_GetLocalAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType* LocalAddrPtr, uint8* NetmaskPtr, TcpIp_SockAddrType* DefaultRouterPtr);
Std_ReturnType SoAd_GetRemoteAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType* IpAddrPtr);
Std_ReturnType SoAd_SetRemoteAddr(SoAd_SoConIdType SoConId, const TcpIp_SockAddrType* RemoteAddrPtr);
Std_ReturnType SoAd_SetUniqueRemoteAddr(SoAd_SoConIdType SoConId, const TcpIp_SockAddrType* RemoteAddrPtr, SoAd_SoConIdType* AssignedSoConIdPtr);
void SoAd_ReleaseRemoteAddr(SoAd_SoConIdType SoConId);
Std_ReturnType SoAd_EnableRouting(SoAd_RoutingGroupIdType id);
Std_ReturnType SoAd_DisableRouting(SoAd_RoutingGroupIdType id);
Std_ReturnType SoAd_EnableSpecificRouting(SoAd_RoutingGroupIdType id, SoAd_SoConIdType SoConId);
Std_ReturnType SoAd_DisableSpecificRouting(SoAd_RoutingGroupIdType id, SoAd_SoConIdType SoConId);
Std_ReturnType SoAd_IfRoutingGroupTransmit(SoAd_RoutingGroupIdType id);
Std_ReturnType SoAd_IfSpecificRoutingGroupTransmit(SoAd_RoutingGroupIdType id, SoAd_SoConIdType SoConId);
TcpIp_ReturnType SoAd_IsConnectionReady(SoAd_SoConIdType SoConId, const TcpIp_SockAddrType* RemoteAddrPtr);
void SoAd_RxIndication(TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr, const uint8* BufPtr, uint16 Length);
BufReq_ReturnType SoAd_CopyTxData(TcpIp_SocketIdType SocketId, uint8* BufPtr, uint16 BufLength);
void SoAd_TxConfirmation(TcpIp_SocketIdType SocketId, uint16 Length);
void SoAd_LocalIpAddrAssignmentChg(TcpIp_LocalAddrIdType IpAddrId, TcpIp_IpAddrStateType State);
Std_ReturnType SoAd_TcpAccepted(TcpIp_SocketIdType SocketId, TcpIp_SocketIdType SocketIdConnected, const TcpIp_SockAddrType* RemoteAddrPtr);
void SoAd_TcpConnected(TcpIp_SocketIdType SocketId);
void SoAd_TcpIpEvent(TcpIp_SocketIdType SocketId, TcpIp_EventType Event);
void SoAd_MainFunction(void);

#ifdef __cplusplus
}
#endif

#endif