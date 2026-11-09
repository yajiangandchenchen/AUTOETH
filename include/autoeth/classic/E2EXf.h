/* AUTOETH Classic C compatibility: E2EXf.h
 * ForwardedCode is absent because Forward is NOT_SELECTED. Each selected
 * binding has an out-of-place and a separate InPlace symbol suffix. */
#ifndef AUTOETH_CLASSIC_E2EXF_H
#define AUTOETH_CLASSIC_E2EXF_H

#include "autoeth/classic/Std_Types.h"
#include "autoeth/classic/SomeIpXf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct E2EXf_CSTransactionHandleType_tag {
  uint32 e2eCounter;
  uint32 e2eSourceId;
} E2EXf_CSTransactionHandleType;

typedef Std_ReturnType (*Std_ExtractProtocolHeaderFieldsType)(const uint8* buffer, uint32 bufferLength, Std_MessageTypeType* messageType, Std_MessageResultType* messageResult);

typedef struct E2EXf_ConfigType_tag {
  uint32 bindingCount;
  const void* bindings;
} E2EXf_ConfigType;

uint8 E2EXf_VehicleEvent(uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_VehicleEvent_InPlace(uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);
uint8 E2EXf_Inv_VehicleEvent(uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_Inv_VehicleEvent_InPlace(uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);

uint8 E2EXf_CalibrationRequest(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, const E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_CalibrationRequest_InPlace(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, const E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);
uint8 E2EXf_Inv_CalibrationRequest(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_Inv_CalibrationRequest_InPlace(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);

uint8 E2EXf_CalibrationResponse(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, const E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_CalibrationResponse_InPlace(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, const E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);
uint8 E2EXf_Inv_CalibrationResponse(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, const uint8* inputBuffer, uint32 inputBufferLength);
uint8 E2EXf_Inv_CalibrationResponse_InPlace(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields, E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength, uint32 inputBufferLength);

void E2EXf_Init(const E2EXf_ConfigType* config);
void E2EXf_DeInit(void);
void E2EXf_GetVersionInfo(Std_VersionInfoType* VersionInfo);

#ifdef __cplusplus
}
#endif

#endif