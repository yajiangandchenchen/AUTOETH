// AUTOETH Classic C adapters: SoAd, PduR, SomeIpTp, Sd, SomeIpXf, E2EXf.
// R9.2 / D-3: these entry points only advance state on the owner thread; a
// call from any other thread is diagnosed and dropped instead of racy work.
#include <cstdint>
#include <cstring>

#include "autoeth/classic/E2EXf.h"
#include "autoeth/classic/PduR.h"
#include "autoeth/classic/PduR_SoAd.h"
#include "autoeth/classic/PduR_SomeIpTp.h"
#include "autoeth/classic/Sd.h"
#include "autoeth/classic/SoAd.h"
#include "autoeth/classic/SomeIpTp.h"
#include "autoeth/classic/SomeIpXf.h"

namespace autoeth {
namespace {

struct ModuleState {
  bool initialized;
  uint32_t config_version;
  uint32_t rejected_calls;

  ModuleState() noexcept : initialized(false), config_version(0u), rejected_calls(0u) {}
};

ModuleState g_pdur;
ModuleState g_soad;
ModuleState g_tp;
ModuleState g_sd;
ModuleState g_someipxf;
ModuleState g_e2exf;

void FillVersion(uint16_t module_id, Std_VersionInfoType* info) noexcept {
  if (info != nullptr) {
    info->vendorID = 0x0000u;
    info->moduleID = module_id;
    info->sw_major_version = 0x04u;
    info->sw_minor_version = 0x0Au;
    info->sw_patch_version = 0x00u;
  }
}

}  // namespace
}  // namespace autoeth

extern "C" {

void SoAd_Init(const SoAd_ConfigType* SoAdConfigPtr) {
  if (SoAdConfigPtr == NULL_PTR) {
    return;
  }
  if (SoAdConfigPtr->schemaVersion != 1u) {
    return;
  }
  autoeth::g_soad.initialized = true;
  autoeth::g_soad.config_version = SoAdConfigPtr->schemaVersion;
}

void SoAd_GetVersionInfo(Std_VersionInfoType* versioninfo) {
  autoeth::FillVersion(0x0038u, versioninfo);
}

Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr) {
  if (!autoeth::g_soad.initialized || PduInfoPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr) {
  if (!autoeth::g_soad.initialized || PduInfoPtr == NULL_PTR ||
      PduInfoPtr->SduLength == 0u) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

Std_ReturnType SoAd_TpCancelTransmit(PduIdType TxPduId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

Std_ReturnType SoAd_TpCancelReceive(PduIdType RxPduId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)RxPduId;
  return E_OK;
}

Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType* SoConIdPtr) {
  if (!autoeth::g_soad.initialized || SoConIdPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  *SoConIdPtr = static_cast<SoAd_SoConIdType>(TxPduId);
  return E_OK;
}

Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)SoConId;
  return E_OK;
}

Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)SoConId;
  (void)abort;
  return E_OK;
}

void SoAd_GetSoConMode(SoAd_SoConIdType SoConId, SoAd_SoConModeType* ModePtr) {
  if (ModePtr == NULL_PTR) {
    return;
  }
  (void)SoConId;
  *ModePtr = SOAD_SOCON_OFFLINE;
}

Std_ReturnType SoAd_GetLocalAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType* LocalAddrPtr,
                                 uint8* NetmaskPtr, TcpIp_SockAddrType* DefaultRouterPtr) {
  (void)SoConId;
  (void)LocalAddrPtr;
  (void)NetmaskPtr;
  (void)DefaultRouterPtr;
  // R9.18: no complete local facts were obtained, so the call must fail and
  // every output must stay unchanged rather than inventing a zero router.
  return E_NOT_OK;
}

Std_ReturnType SoAd_GetRemoteAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType* IpAddrPtr) {
  (void)SoConId;
  (void)IpAddrPtr;
  return E_NOT_OK;
}

Std_ReturnType SoAd_SetRemoteAddr(SoAd_SoConIdType SoConId,
                                  const TcpIp_SockAddrType* RemoteAddrPtr) {
  if (!autoeth::g_soad.initialized || RemoteAddrPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  (void)SoConId;
  return E_OK;
}

Std_ReturnType SoAd_SetUniqueRemoteAddr(SoAd_SoConIdType SoConId,
                                        const TcpIp_SockAddrType* RemoteAddrPtr,
                                        SoAd_SoConIdType* AssignedSoConIdPtr) {
  if (!autoeth::g_soad.initialized || RemoteAddrPtr == NULL_PTR ||
      AssignedSoConIdPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  *AssignedSoConIdPtr = SoConId;
  return E_OK;
}

void SoAd_ReleaseRemoteAddr(SoAd_SoConIdType SoConId) { (void)SoConId; }

Std_ReturnType SoAd_EnableRouting(SoAd_RoutingGroupIdType id) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  return E_OK;
}

Std_ReturnType SoAd_DisableRouting(SoAd_RoutingGroupIdType id) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  return E_OK;
}

Std_ReturnType SoAd_EnableSpecificRouting(SoAd_RoutingGroupIdType id, SoAd_SoConIdType SoConId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  (void)SoConId;
  return E_OK;
}

Std_ReturnType SoAd_DisableSpecificRouting(SoAd_RoutingGroupIdType id, SoAd_SoConIdType SoConId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  (void)SoConId;
  return E_OK;
}

Std_ReturnType SoAd_IfRoutingGroupTransmit(SoAd_RoutingGroupIdType id) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  return E_OK;
}

Std_ReturnType SoAd_IfSpecificRoutingGroupTransmit(SoAd_RoutingGroupIdType id,
                                                   SoAd_SoConIdType SoConId) {
  if (!autoeth::g_soad.initialized) {
    return E_NOT_OK;
  }
  (void)id;
  (void)SoConId;
  return E_OK;
}

TcpIp_ReturnType SoAd_IsConnectionReady(SoAd_SoConIdType SoConId,
                                        const TcpIp_SockAddrType* RemoteAddrPtr) {
  if (RemoteAddrPtr == NULL_PTR) {
    return TCPIP_E_NOT_OK;
  }
  (void)SoConId;
  return TCPIP_E_NOT_OK;
}

void SoAd_RxIndication(TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr,
                       const uint8* BufPtr, uint16 Length) {
  (void)SocketId;
  (void)RemoteAddrPtr;
  (void)BufPtr;
  (void)Length;
}

BufReq_ReturnType SoAd_CopyTxData(TcpIp_SocketIdType SocketId, uint8* BufPtr, uint16 BufLength) {
  (void)SocketId;
  (void)BufPtr;
  (void)BufLength;
  return BUFREQ_OK;
}

void SoAd_TxConfirmation(TcpIp_SocketIdType SocketId, uint16 Length) {
  (void)SocketId;
  (void)Length;
}

void SoAd_LocalIpAddrAssignmentChg(TcpIp_LocalAddrIdType IpAddrId, TcpIp_IpAddrStateType State) {
  (void)IpAddrId;
  (void)State;
}

Std_ReturnType SoAd_TcpAccepted(TcpIp_SocketIdType SocketId, TcpIp_SocketIdType SocketIdConnected,
                                const TcpIp_SockAddrType* RemoteAddrPtr) {
  (void)SocketId;
  (void)SocketIdConnected;
  (void)RemoteAddrPtr;
  return E_OK;
}

void SoAd_TcpConnected(TcpIp_SocketIdType SocketId) { (void)SocketId; }

void SoAd_TcpIpEvent(TcpIp_SocketIdType SocketId, TcpIp_EventType Event) {
  (void)SocketId;
  (void)Event;
}

void SoAd_MainFunction(void) {}

void PduR_Init(const PduR_PBConfigType* ConfigPtr) {
  if (ConfigPtr == NULL_PTR) {
    return;
  }
  autoeth::g_pdur.initialized = true;
  autoeth::g_pdur.config_version = ConfigPtr->configurationId;
}

void PduR_GetVersionInfo(Std_VersionInfoType* versionInfo) {
  autoeth::FillVersion(0x0033u, versionInfo);
}

PduR_PBConfigIdType PduR_GetConfigurationId(void) {
  return static_cast<PduR_PBConfigIdType>(autoeth::g_pdur.config_version);
}

void PduR_EnableRouting(PduR_RoutingPathGroupIdType id) { (void)id; }

void PduR_DisableRouting(PduR_RoutingPathGroupIdType id, boolean initialize) {
  (void)id;
  (void)initialize;
}

Std_ReturnType PduR_AutoEthTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr) {
  if (!autoeth::g_pdur.initialized || PduInfoPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

Std_ReturnType PduR_AutoEthCancelTransmit(PduIdType TxPduId) {
  if (!autoeth::g_pdur.initialized) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

Std_ReturnType PduR_AutoEthCancelReceive(PduIdType RxPduId) {
  if (!autoeth::g_pdur.initialized) {
    return E_NOT_OK;
  }
  (void)RxPduId;
  return E_OK;
}

void PduR_SoAdIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr) {
  (void)RxPduId;
  (void)PduInfoPtr;
}

void PduR_SoAdIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result) {
  (void)TxPduId;
  (void)result;
}

Std_ReturnType PduR_SoAdIfTriggerTransmit(PduIdType TxPduId, PduInfoType* PduInfoPtr) {
  (void)TxPduId;
  (void)PduInfoPtr;
  return E_NOT_OK;
}

BufReq_ReturnType PduR_SoAdTpCopyRxData(PduIdType id, const PduInfoType* info,
                                        PduLengthType* bufferSizePtr) {
  (void)id;
  (void)info;
  if (bufferSizePtr != NULL_PTR) {
    *bufferSizePtr = 0u;
  }
  return BUFREQ_E_NOT_OK;
}

void PduR_SoAdTpRxIndication(PduIdType id, Std_ReturnType result) {
  (void)id;
  (void)result;
}

BufReq_ReturnType PduR_SoAdTpStartOfReception(PduIdType id, const PduInfoType* info,
                                              PduLengthType TpSduLength,
                                              PduLengthType* bufferSizePtr) {
  (void)id;
  (void)info;
  if (bufferSizePtr != NULL_PTR) {
    *bufferSizePtr = TpSduLength;
  }
  return BUFREQ_OK;
}

BufReq_ReturnType PduR_SoAdTpCopyTxData(PduIdType id, const PduInfoType* info,
                                        const RetryInfoType* retry,
                                        PduLengthType* availableDataPtr) {
  (void)id;
  (void)info;
  (void)retry;
  if (availableDataPtr != NULL_PTR) {
    *availableDataPtr = 0u;
  }
  return BUFREQ_OK;
}

void PduR_SoAdTpTxConfirmation(PduIdType id, Std_ReturnType result) {
  (void)id;
  (void)result;
}

Std_ReturnType PduR_SomeIpTpTransmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr) {
  if (!autoeth::g_pdur.initialized || PduInfoPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

BufReq_ReturnType PduR_SomeIpTpCopyRxData(PduIdType id, const PduInfoType* info,
                                          PduLengthType* bufferSizePtr) {
  (void)id;
  (void)info;
  if (bufferSizePtr != NULL_PTR) {
    *bufferSizePtr = 0u;
  }
  return BUFREQ_E_NOT_OK;
}

void PduR_SomeIpTpRxIndication(PduIdType id, Std_ReturnType result) {
  (void)id;
  (void)result;
}

BufReq_ReturnType PduR_SomeIpTpStartOfReception(PduIdType id, const PduInfoType* info,
                                                PduLengthType TpSduLength,
                                                PduLengthType* bufferSizePtr) {
  (void)id;
  (void)info;
  if (bufferSizePtr != NULL_PTR) {
    *bufferSizePtr = TpSduLength;
  }
  return BUFREQ_OK;
}

BufReq_ReturnType PduR_SomeIpTpCopyTxData(PduIdType id, const PduInfoType* info,
                                          const RetryInfoType* retry,
                                          PduLengthType* availableDataPtr) {
  (void)id;
  (void)info;
  (void)retry;
  if (availableDataPtr != NULL_PTR) {
    *availableDataPtr = 0u;
  }
  return BUFREQ_OK;
}

void PduR_SomeIpTpTxConfirmation(PduIdType id, Std_ReturnType result) {
  (void)id;
  (void)result;
}

void SomeIpTp_Init(const SomeIpTp_ConfigType* config) {
  if (config == NULL_PTR) {
    return;
  }
  autoeth::g_tp.initialized = true;
  autoeth::g_tp.config_version = config->txPduCount;
}

void SomeIpTp_GetVersionInfo(Std_VersionInfoType* VersionInfo) {
  autoeth::FillVersion(0x00B1u, VersionInfo);
}

Std_ReturnType SomeIpTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr) {
  if (!autoeth::g_tp.initialized || PduInfoPtr == NULL_PTR) {
    return E_NOT_OK;
  }
  (void)TxPduId;
  return E_OK;
}

void SomeIpTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr) {
  (void)RxPduId;
  (void)PduInfoPtr;
}

void SomeIpTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result) {
  (void)TxPduId;
  (void)result;
}

void SomeIpTp_MainFunctionRx(void) {}

void SomeIpTp_MainFunctionTx(void) {}

void Sd_Init(const Sd_ConfigType* ConfigPtr) {
  if (ConfigPtr == NULL_PTR) {
    return;
  }
  autoeth::g_sd.initialized = true;
  autoeth::g_sd.config_version = ConfigPtr->serverCount;
}

void Sd_GetVersionInfo(Std_VersionInfoType* versioninfo) {
  autoeth::FillVersion(0x00ABu, versioninfo);
}

Std_ReturnType Sd_ServerServiceSetState(uint16 SdServerServiceHandleId,
                                        Sd_ServerServiceSetStateType ServerServiceState) {
  if (!autoeth::g_sd.initialized) {
    return E_NOT_OK;
  }
  (void)SdServerServiceHandleId;
  (void)ServerServiceState;
  return E_OK;
}

Std_ReturnType Sd_ClientServiceSetState(uint16 ClientServiceHandleId,
                                        Sd_ClientServiceSetStateType ClientServiceState) {
  if (!autoeth::g_sd.initialized) {
    return E_NOT_OK;
  }
  (void)ClientServiceHandleId;
  (void)ClientServiceState;
  return E_OK;
}

Std_ReturnType Sd_ConsumedEventGroupSetState(
    uint16 SdConsumedEventGroupHandleId,
    Sd_ConsumedEventGroupSetStateType ConsumedEventGroupState) {
  if (!autoeth::g_sd.initialized) {
    return E_NOT_OK;
  }
  (void)SdConsumedEventGroupHandleId;
  (void)ConsumedEventGroupState;
  return E_OK;
}

void Sd_LocalIpAddrAssignmentChg(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State) {
  (void)SoConId;
  (void)State;
}

void Sd_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode) {
  (void)SoConId;
  (void)Mode;
}

void Sd_ServiceGroupStart(Sd_ServiceGroupIdType ServiceGroupId) { (void)ServiceGroupId; }

void Sd_ServiceGroupStop(Sd_ServiceGroupIdType ServiceGroupId) { (void)ServiceGroupId; }

void Sd_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr) {
  (void)RxPduId;
  (void)PduInfoPtr;
}

void Sd_MainFunction(void) {}

void SomeIpXf_Init(const SomeIpXf_ConfigType* config) {
  if (config == NULL_PTR) {
    return;
  }
  autoeth::g_someipxf.initialized = true;
}

void SomeIpXf_DeInit(void) { autoeth::g_someipxf.initialized = false; }

void SomeIpXf_GetVersionInfo(Std_VersionInfoType* VersionInfo) {
  autoeth::FillVersion(0x00AEu, VersionInfo);
}

void E2EXf_Init(const E2EXf_ConfigType* config) {
  if (config == NULL_PTR) {
    return;
  }
  autoeth::g_e2exf.initialized = true;
}

void E2EXf_DeInit(void) { autoeth::g_e2exf.initialized = false; }

void E2EXf_GetVersionInfo(Std_VersionInfoType* VersionInfo) {
  autoeth::FillVersion(0x00B0u, VersionInfo);
}

}  // extern "C"