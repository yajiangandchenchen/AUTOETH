/* AUTOETH Classic C compatibility: Crc.h
 * Selected CRC APIs only: CRC8, CRC8H2F, CRC16, CRC32P4, CRC64 and version. */
#ifndef AUTOETH_CLASSIC_CRC_H
#define AUTOETH_CLASSIC_CRC_H

#include "autoeth/classic/Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8 Crc_CalculateCRC8(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8, boolean Crc_IsFirstCall);
uint8 Crc_CalculateCRC8H2F(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8H2F, boolean Crc_IsFirstCall);
uint16 Crc_CalculateCRC16(const uint8* Crc_DataPtr, uint32 Crc_Length, uint16 Crc_StartValue16, boolean Crc_IsFirstCall);
uint32 Crc_CalculateCRC32P4(const uint8* Crc_DataPtr, uint32 Crc_Length, uint32 Crc_StartValue32, boolean Crc_IsFirstCall);
uint64 Crc_CalculateCRC64(const uint8* Crc_DataPtr, uint32 Crc_Length, uint64 Crc_StartValue64, boolean Crc_IsFirstCall);
void Crc_GetVersionInfo(Std_VersionInfoType* Versioninfo);

#ifdef __cplusplus
}
#endif

#endif