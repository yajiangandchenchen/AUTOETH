/* AUTOETH Classic C compatibility: Sd.h
 * C SD bindings reference the Native immutable descriptions and timers; they
 * are not a second SD engine. Current-state enums keep their source values. */
#ifndef AUTOETH_CLASSIC_SD_H
#define AUTOETH_CLASSIC_SD_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/ComStack_Types.h"
#include "autoeth/classic/SoAd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16 Sd_ServiceGroupIdType;

typedef uint8 Sd_ServerServiceSetStateType;
#define SD_SERVER_SERVICE_DOWN ((Sd_ServerServiceSetStateType)0u)
#define SD_SERVER_SERVICE_REQUESTED ((Sd_ServerServiceSetStateType)1u)
#define SD_SERVER_SERVICE_RELEASED ((Sd_ServerServiceSetStateType)2u)

typedef uint8 Sd_ClientServiceSetStateType;
#define SD_CLIENT_SERVICE_DOWN ((Sd_ClientServiceSetStateType)0u)
#define SD_CLIENT_SERVICE_REQUESTED ((Sd_ClientServiceSetStateType)1u)
#define SD_CLIENT_SERVICE_RELEASED ((Sd_ClientServiceSetStateType)2u)

typedef uint8 Sd_ConsumedEventGroupSetStateType;
#define SD_CONSUMED_EVENTGROUP_DOWN ((Sd_ConsumedEventGroupSetStateType)0u)
#define SD_CONSUMED_EVENTGROUP_REQUESTED ((Sd_ConsumedEventGroupSetStateType)1u)
#define SD_CONSUMED_EVENTGROUP_RELEASED ((Sd_ConsumedEventGroupSetStateType)2u)

typedef uint8 Sd_ClientServiceCurrentStateType;
#define SD_CLIENT_SERVICE_CURRENT_DOWN ((Sd_ClientServiceCurrentStateType)0u)
#define SD_CLIENT_SERVICE_CURRENT_AVAILABLE ((Sd_ClientServiceCurrentStateType)1u)

typedef uint8 Sd_ConsumedEventGroupCurrentStateType;
#define SD_CONSUMED_EVENTGROUP_CURRENT_DOWN ((Sd_ConsumedEventGroupCurrentStateType)0u)
#define SD_CONSUMED_EVENTGROUP_CURRENT_AVAILABLE ((Sd_ConsumedEventGroupCurrentStateType)1u)

typedef uint8 Sd_EventHandlerCurrentStateType;
#define SD_EVENT_HANDLER_CURRENT_DOWN ((Sd_EventHandlerCurrentStateType)0u)
#define SD_EVENT_HANDLER_CURRENT_AVAILABLE ((Sd_EventHandlerCurrentStateType)1u)

typedef struct AeSdServerRefType_tag {
  uint16 handleId;
  uint32 nativeServiceIndex;
  Sd_ServerServiceSetStateType initialRequested;
} AeSdServerRef;

typedef struct AeSdClientRefType_tag {
  uint16 handleId;
  uint32 nativeQueryIndex;
  Sd_ClientServiceSetStateType initialRequested;
} AeSdClientRef;

typedef struct AeSdConsumedGroupRefType_tag {
  uint16 handleId;
  uint16 nativeClientHandle;
  uint16 nativeEventGroupId;
  Sd_ConsumedEventGroupSetStateType initialRequested;
} AeSdConsumedGroupRef;

typedef struct AeSdServiceGroupType_tag {
  uint16 handleId;
  uint16 serverRefCount;
  const uint16* serverRefs;
  uint16 clientRefCount;
  const uint16* clientRefs;
} AeSdServiceGroup;

typedef struct AeSdCurrentCallbacksType_tag {
  void (*ClientServiceCurrentState)(uint16, Sd_ClientServiceCurrentStateType);
  void (*ConsumedEventGroupCurrentState)(uint16, Sd_ConsumedEventGroupCurrentStateType);
  void (*EventHandlerCurrentState)(uint16, Sd_EventHandlerCurrentStateType);
} AeSdCurrentCallbacks;

typedef struct Sd_ConfigType_tag {
  uint32 serverCount;
  const AeSdServerRef* servers;
  uint32 clientCount;
  const AeSdClientRef* clients;
  uint32 consumedGroupCount;
  const AeSdConsumedGroupRef* consumedGroups;
  uint32 serviceGroupCount;
  const AeSdServiceGroup* serviceGroups;
  AeSdCurrentCallbacks currentCallbacks;
} Sd_ConfigType;

void Sd_Init(const Sd_ConfigType* ConfigPtr);
void Sd_GetVersionInfo(Std_VersionInfoType* versioninfo);
Std_ReturnType Sd_ServerServiceSetState(uint16 SdServerServiceHandleId, Sd_ServerServiceSetStateType ServerServiceState);
Std_ReturnType Sd_ClientServiceSetState(uint16 ClientServiceHandleId, Sd_ClientServiceSetStateType ClientServiceState);
Std_ReturnType Sd_ConsumedEventGroupSetState(uint16 SdConsumedEventGroupHandleId, Sd_ConsumedEventGroupSetStateType ConsumedEventGroupState);
void Sd_LocalIpAddrAssignmentChg(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State);
void Sd_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode);
void Sd_ServiceGroupStart(Sd_ServiceGroupIdType ServiceGroupId);
void Sd_ServiceGroupStop(Sd_ServiceGroupIdType ServiceGroupId);
void Sd_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
void Sd_MainFunction(void);

#ifdef __cplusplus
}
#endif

#endif