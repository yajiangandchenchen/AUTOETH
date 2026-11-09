/* AUTOETH Classic C compatibility: SomeIpXf.h
 * Concrete project serializer adapters calling the same production codec.
 * No RTE generator is implied. Return bytes 00/01/81/87/88/89/8A stay in the
 * source SomeIpXf domain and are separate from local Status/wire codes. */
#ifndef AUTOETH_CLASSIC_SOMEIPXF_H
#define AUTOETH_CLASSIC_SOMEIPXF_H

#include "autoeth/classic/Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUTOETH_LABEL_CAPACITY 72u

typedef struct Rte_Cs_TransactionHandleType_tag {
  uint16 clientId;
  uint16 sequenceCounter;
} Rte_Cs_TransactionHandleType;

typedef struct AeVehicleSnapshotType_tag {
  uint32 revision;
  uint16 speedCenti;
  sint16 temperatureDeci;
  uint8 flags;
  uint8 label[AUTOETH_LABEL_CAPACITY];
  uint32 labelUnits;
} AeVehicleSnapshot;

typedef struct AeCalibrationType_tag {
  uint16 speedLimitCenti;
  uint16 gainMilli;
  uint8 label[AUTOETH_LABEL_CAPACITY];
  uint32 labelUnits;
} AeCalibration;

typedef struct AeCalibrationStateType_tag {
  uint32 revision;
  AeCalibration value;
} AeCalibrationState;

typedef struct SomeIpXf_ConfigType_tag {
  uint32 bindingCount;
  const void* bindings;
} SomeIpXf_ConfigType;

Std_ReturnType SomeIpXf_ExtractProtocolHeaderFields(const uint8* buffer, uint32 bufferLength, Std_MessageTypeType* messageType, Std_MessageResultType* messageResult);

uint8 SomeIpXf_VehicleEvent(uint8* buffer, uint32* bufferLength, const AeVehicleSnapshot* dataElement);
uint8 SomeIpXf_Inv_VehicleEvent(const uint8* buffer, uint32 bufferLength, AeVehicleSnapshot* dataElement);
uint8 SomeIpXf_CalibrationRequest(const Rte_Cs_TransactionHandleType* TransactionHandle, uint8* buffer, uint32* bufferLength, const AeCalibration* dataElement);
uint8 SomeIpXf_Inv_CalibrationRequest(Rte_Cs_TransactionHandleType* TransactionHandle, const uint8* buffer, uint32 bufferLength, AeCalibration* dataElement);
uint8 SomeIpXf_CalibrationResponse(const Rte_Cs_TransactionHandleType* TransactionHandle, uint8* buffer, uint32* bufferLength, Std_ReturnType returnValue, const AeCalibrationState* dataElement);
uint8 SomeIpXf_Inv_CalibrationResponse(Rte_Cs_TransactionHandleType* TransactionHandle, const uint8* buffer, uint32 bufferLength, Std_ReturnType* returnValue, AeCalibrationState* dataElement);

void SomeIpXf_Init(const SomeIpXf_ConfigType* config);
void SomeIpXf_DeInit(void);
void SomeIpXf_GetVersionInfo(Std_VersionInfoType* VersionInfo);

#ifdef __cplusplus
}
#endif

#endif