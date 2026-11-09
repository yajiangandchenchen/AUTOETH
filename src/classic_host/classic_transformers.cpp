// AUTOETH Classic transformer adapters (SomeIpXf and E2EXf).
// R9.15: a standalone call without the required bound RPC context returns the
// hard runtime byte 0xFF and leaves every output untouched.
#include <cstring>

#include "autoeth/classic/E2EXf.h"
#include "autoeth/classic/SomeIpXf.h"

namespace {

const uint8 kSomeIpXfOk = 0x00u;
const uint8 kSomeIpXfErrSerialization = 0x81u;
const uint8 kSomeIpXfNoBoundContext = 0xFFu;

void WriteU32Big(uint8* buffer, uint32 offset, uint32 value) noexcept {
  buffer[offset] = static_cast<uint8>((value >> 24u) & 0xFFu);
  buffer[offset + 1u] = static_cast<uint8>((value >> 16u) & 0xFFu);
  buffer[offset + 2u] = static_cast<uint8>((value >> 8u) & 0xFFu);
  buffer[offset + 3u] = static_cast<uint8>(value & 0xFFu);
}

void WriteU16Big(uint8* buffer, uint32 offset, uint16 value) noexcept {
  buffer[offset] = static_cast<uint8>((value >> 8u) & 0xFFu);
  buffer[offset + 1u] = static_cast<uint8>(value & 0xFFu);
}

uint32 ReadU32Big(const uint8* buffer, uint32 offset) noexcept {
  const uint32 b0 = buffer[offset];
  const uint32 b1 = buffer[offset + 1u];
  const uint32 b2 = buffer[offset + 2u];
  const uint32 b3 = buffer[offset + 3u];
  return (b0 << 24u) | (b1 << 16u) | (b2 << 8u) | b3;
}

uint16 ReadU16Big(const uint8* buffer, uint32 offset) noexcept {
  const uint32 b0 = buffer[offset];
  const uint32 b1 = buffer[offset + 1u];
  return static_cast<uint16>((b0 << 8u) | b1);
}

const uint32 kVehicleSnapshotWireBytes = 4u + 2u + 2u + 1u + 72u + 4u;
const uint32 kCalibrationWireBytes = 2u + 2u + 72u + 4u;

bool CopyLabel(const uint8* source, uint32 units, uint8* destination, uint32 capacity) noexcept {
  if (units > capacity) {
    return false;
  }
  for (uint32 index = 0u; index < units; ++index) {
    destination[index] = source[index];
  }
  return true;
}

}  // namespace

extern "C" {

Std_ReturnType SomeIpXf_ExtractProtocolHeaderFields(const uint8* buffer, uint32 bufferLength,
                                                    Std_MessageTypeType* messageType,
                                                    Std_MessageResultType* messageResult) {
  if (messageType == NULL_PTR || messageResult == NULL_PTR) {
    return E_NOT_OK;
  }
  if (buffer == NULL_PTR || bufferLength < 16u) {
    return E_NOT_OK;
  }
  *messageType = static_cast<Std_MessageTypeType>(buffer[14u]);
  const uint8 returnCode = buffer[15u];
  if (returnCode != 0u) {
    *messageResult = STD_MESSAGERESULT_ERROR;
  } else {
    *messageResult = STD_MESSAGERESULT_OK;
  }
  return E_OK;
}

uint8 SomeIpXf_VehicleEvent(uint8* buffer, uint32* bufferLength,
                            const AeVehicleSnapshot* dataElement) {
  if (buffer == NULL_PTR || bufferLength == NULL_PTR || dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (*bufferLength < kVehicleSnapshotWireBytes) {
    return kSomeIpXfErrSerialization;
  }
  if (dataElement->labelUnits > AUTOETH_LABEL_CAPACITY) {
    return kSomeIpXfErrSerialization;
  }
  WriteU32Big(buffer, 0u, dataElement->revision);
  WriteU16Big(buffer, 4u, dataElement->speedCenti);
  WriteU16Big(buffer, 6u, static_cast<uint16>(dataElement->temperatureDeci));
  buffer[8u] = dataElement->flags;
  if (!CopyLabel(dataElement->label, dataElement->labelUnits, buffer + 9u,
                 AUTOETH_LABEL_CAPACITY)) {
    return kSomeIpXfErrSerialization;
  }
  WriteU32Big(buffer, 9u + AUTOETH_LABEL_CAPACITY, dataElement->labelUnits);
  *bufferLength = kVehicleSnapshotWireBytes;
  return kSomeIpXfOk;
}

uint8 SomeIpXf_Inv_VehicleEvent(const uint8* buffer, uint32 bufferLength,
                                AeVehicleSnapshot* dataElement) {
  if (buffer == NULL_PTR || dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (bufferLength < kVehicleSnapshotWireBytes) {
    return kSomeIpXfErrSerialization;
  }
  dataElement->revision = ReadU32Big(buffer, 0u);
  dataElement->speedCenti = ReadU16Big(buffer, 4u);
  dataElement->temperatureDeci = static_cast<sint16>(ReadU16Big(buffer, 6u));
  dataElement->flags = buffer[8u];
  const uint32 units = ReadU32Big(buffer, 9u + AUTOETH_LABEL_CAPACITY);
  if (units > AUTOETH_LABEL_CAPACITY) {
    return kSomeIpXfErrSerialization;
  }
  for (uint32 index = 0u; index < units; ++index) {
    dataElement->label[index] = buffer[9u + index];
  }
  dataElement->labelUnits = units;
  return kSomeIpXfOk;
}

uint8 SomeIpXf_CalibrationRequest(const Rte_Cs_TransactionHandleType* TransactionHandle,
                                  uint8* buffer, uint32* bufferLength,
                                  const AeCalibration* dataElement) {
  if (TransactionHandle == NULL_PTR || buffer == NULL_PTR || bufferLength == NULL_PTR ||
      dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (*bufferLength < kCalibrationWireBytes || dataElement->labelUnits > AUTOETH_LABEL_CAPACITY) {
    return kSomeIpXfErrSerialization;
  }
  WriteU16Big(buffer, 0u, dataElement->speedLimitCenti);
  WriteU16Big(buffer, 2u, dataElement->gainMilli);
  if (!CopyLabel(dataElement->label, dataElement->labelUnits, buffer + 4u, AUTOETH_LABEL_CAPACITY)) {
    return kSomeIpXfErrSerialization;
  }
  WriteU32Big(buffer, 4u + AUTOETH_LABEL_CAPACITY, dataElement->labelUnits);
  *bufferLength = kCalibrationWireBytes;
  return kSomeIpXfOk;
}

uint8 SomeIpXf_Inv_CalibrationRequest(Rte_Cs_TransactionHandleType* TransactionHandle,
                                      const uint8* buffer, uint32 bufferLength,
                                      AeCalibration* dataElement) {
  if (TransactionHandle == NULL_PTR || buffer == NULL_PTR || dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (bufferLength < kCalibrationWireBytes) {
    return kSomeIpXfErrSerialization;
  }
  dataElement->speedLimitCenti = ReadU16Big(buffer, 0u);
  dataElement->gainMilli = ReadU16Big(buffer, 2u);
  const uint32 units = ReadU32Big(buffer, 4u + AUTOETH_LABEL_CAPACITY);
  if (units > AUTOETH_LABEL_CAPACITY) {
    return kSomeIpXfErrSerialization;
  }
  for (uint32 index = 0u; index < units; ++index) {
    dataElement->label[index] = buffer[4u + index];
  }
  dataElement->labelUnits = units;
  return kSomeIpXfOk;
}

uint8 SomeIpXf_CalibrationResponse(const Rte_Cs_TransactionHandleType* TransactionHandle,
                                   uint8* buffer, uint32* bufferLength, Std_ReturnType returnValue,
                                   const AeCalibrationState* dataElement) {
  if (TransactionHandle == NULL_PTR || buffer == NULL_PTR || bufferLength == NULL_PTR ||
      dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (*bufferLength < 4u + kCalibrationWireBytes) {
    return kSomeIpXfErrSerialization;
  }
  buffer[0u] = returnValue;
  WriteU32Big(buffer, 4u, dataElement->revision);
  WriteU16Big(buffer, 8u, dataElement->value.speedLimitCenti);
  WriteU16Big(buffer, 10u, dataElement->value.gainMilli);
  if (!CopyLabel(dataElement->value.label, dataElement->value.labelUnits, buffer + 12u,
                 AUTOETH_LABEL_CAPACITY)) {
    return kSomeIpXfErrSerialization;
  }
  WriteU32Big(buffer, 12u + AUTOETH_LABEL_CAPACITY, dataElement->value.labelUnits);
  *bufferLength = 4u + kVehicleSnapshotWireBytes - 8u;
  return kSomeIpXfOk;
}

uint8 SomeIpXf_Inv_CalibrationResponse(Rte_Cs_TransactionHandleType* TransactionHandle,
                                       const uint8* buffer, uint32 bufferLength,
                                       Std_ReturnType* returnValue,
                                       AeCalibrationState* dataElement) {
  if (TransactionHandle == NULL_PTR || buffer == NULL_PTR || returnValue == NULL_PTR ||
      dataElement == NULL_PTR) {
    return kSomeIpXfErrSerialization;
  }
  if (bufferLength < 4u + kCalibrationWireBytes) {
    return kSomeIpXfErrSerialization;
  }
  *returnValue = buffer[0u];
  dataElement->revision = ReadU32Big(buffer, 4u);
  dataElement->value.speedLimitCenti = ReadU16Big(buffer, 8u);
  dataElement->value.gainMilli = ReadU16Big(buffer, 10u);
  const uint32 units = ReadU32Big(buffer, 12u + AUTOETH_LABEL_CAPACITY);
  if (units > AUTOETH_LABEL_CAPACITY) {
    return kSomeIpXfErrSerialization;
  }
  for (uint32 index = 0u; index < units; ++index) {
    dataElement->value.label[index] = buffer[12u + index];
  }
  dataElement->value.labelUnits = units;
  return kSomeIpXfOk;
}

uint8 E2EXf_VehicleEvent(uint8* buffer, uint32* bufferLength, const uint8* inputBuffer,
                         uint32 inputBufferLength) {
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_VehicleEvent_InPlace(uint8* buffer, uint32* bufferLength, uint32 inputBufferLength) {
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_VehicleEvent(uint8* buffer, uint32* bufferLength, const uint8* inputBuffer,
                             uint32 inputBufferLength) {
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_VehicleEvent_InPlace(uint8* buffer, uint32* bufferLength,
                                     uint32 inputBufferLength) {
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_CalibrationRequest(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
                               const E2EXf_CSTransactionHandleType* csTransactionHandle,
                               uint8* buffer, uint32* bufferLength, const uint8* inputBuffer,
                               uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_CalibrationRequest_InPlace(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
                                       const E2EXf_CSTransactionHandleType* csTransactionHandle,
                                       uint8* buffer, uint32* bufferLength,
                                       uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_CalibrationRequest(
    Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
    E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength,
    const uint8* inputBuffer, uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_CalibrationRequest_InPlace(
    Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
    E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength,
    uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_CalibrationResponse(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
                                const E2EXf_CSTransactionHandleType* csTransactionHandle,
                                uint8* buffer, uint32* bufferLength, const uint8* inputBuffer,
                                uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_CalibrationResponse_InPlace(
    Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
    const E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength,
    uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_CalibrationResponse(Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
                                    E2EXf_CSTransactionHandleType* csTransactionHandle,
                                    uint8* buffer, uint32* bufferLength, const uint8* inputBuffer,
                                    uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBuffer;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

uint8 E2EXf_Inv_CalibrationResponse_InPlace(
    Std_ExtractProtocolHeaderFieldsType extractProtocolHeaderFields,
    E2EXf_CSTransactionHandleType* csTransactionHandle, uint8* buffer, uint32* bufferLength,
    uint32 inputBufferLength) {
  (void)extractProtocolHeaderFields;
  (void)csTransactionHandle;
  (void)buffer;
  (void)bufferLength;
  (void)inputBufferLength;
  return kSomeIpXfNoBoundContext;
}

}  // extern "C"