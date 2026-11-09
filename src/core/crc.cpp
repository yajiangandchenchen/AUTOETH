// AUTOETH core: selected CRC routines (CRC8, CRC8H2F, CRC16, CRC32P4, CRC64).
// Table-free bitwise implementation keeps the code verifiable and avoids any
// dynamic storage allocation.
#include <cstdint>

#include "autoeth/classic/Crc.h"

namespace core_detail {

uint8_t Crc8Reflect(uint8_t value) noexcept {
  uint8_t result = 0u;
  for (uint32_t bit = 0u; bit < 8u; ++bit) {
    result = static_cast<uint8_t>(result << 1u);
    if ((value & 0x01u) != 0u) {
      result = static_cast<uint8_t>(result | 0x01u);
    }
    value = static_cast<uint8_t>(value >> 1u);
  }
  return result;
}

uint32_t Crc32Reflect(uint32_t value) noexcept {
  uint32_t result = 0u;
  for (uint32_t bit = 0u; bit < 32u; ++bit) {
    result = result << 1u;
    if ((value & 0x00000001u) != 0u) {
      result = result | 0x00000001u;
    }
    value = value >> 1u;
  }
  return result;
}

uint64_t Crc64Reflect(uint64_t value) noexcept {
  uint64_t result = 0u;
  for (uint32_t bit = 0u; bit < 64u; ++bit) {
    result = result << 1u;
    if ((value & 0x0000000000000001u) != 0u) {
      result = result | 0x0000000000000001u;
    }
    value = value >> 1u;
  }
  return result;
}

}  // namespace core_detail

using core_detail::Crc8Reflect;
using core_detail::Crc32Reflect;
using core_detail::Crc64Reflect;

extern "C" {

uint8 Crc_CalculateCRC8(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8,
                        boolean Crc_IsFirstCall) {
  uint8_t crc = (Crc_IsFirstCall != static_cast<boolean>(0)) ? 0xFFu : Crc_StartValue8;
  if (Crc_DataPtr == NULL_PTR && Crc_Length != 0u) {
    return 0u;
  }
  for (uint32_t index = 0u; index < Crc_Length; ++index) {
    crc = static_cast<uint8_t>(crc ^ Crc_DataPtr[index]);
    for (uint32_t bit = 0u; bit < 8u; ++bit) {
      if ((crc & 0x80u) != 0u) {
        crc = static_cast<uint8_t>((crc << 1u) ^ 0x1Du);
      } else {
        crc = static_cast<uint8_t>(crc << 1u);
      }
    }
  }
  return static_cast<uint8>(crc ^ 0xFFu);
}

uint8 Crc_CalculateCRC8H2F(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8H2F,
                           boolean Crc_IsFirstCall) {
  uint8_t crc = (Crc_IsFirstCall != static_cast<boolean>(0)) ? 0xFFu : Crc_StartValue8H2F;
  if (Crc_DataPtr == NULL_PTR && Crc_Length != 0u) {
    return 0u;
  }
  for (uint32_t index = 0u; index < Crc_Length; ++index) {
    crc = Crc8Reflect(static_cast<uint8_t>(crc ^ Crc_DataPtr[index]));
    uint32_t x = static_cast<uint32_t>(crc);
    for (uint32_t bit = 0u; bit < 8u; ++bit) {
      if ((x & 0x80u) != 0u) {
        x = (x << 1u) ^ 0x2Fu;
      } else {
        x = x << 1u;
      }
    }
    crc = static_cast<uint8_t>(Crc8Reflect(static_cast<uint8_t>(x)));
  }
  return static_cast<uint8>(crc ^ 0xFFu);
}

uint16 Crc_CalculateCRC16(const uint8* Crc_DataPtr, uint32 Crc_Length, uint16 Crc_StartValue16,
                          boolean Crc_IsFirstCall) {
  uint16_t crc = (Crc_IsFirstCall != static_cast<boolean>(0)) ? 0xFFFFu : Crc_StartValue16;
  if (Crc_DataPtr == NULL_PTR && Crc_Length != 0u) {
    return 0u;
  }
  for (uint32_t index = 0u; index < Crc_Length; ++index) {
    crc = static_cast<uint16_t>(crc ^ Crc_DataPtr[index]);
    for (uint32_t bit = 0u; bit < 8u; ++bit) {
      if ((crc & 0x8000u) != 0u) {
        crc = static_cast<uint16_t>((crc << 1u) ^ 0x8005u);
      } else {
        crc = static_cast<uint16_t>(crc << 1u);
      }
    }
  }
  return static_cast<uint16>(crc);
}

uint32 Crc_CalculateCRC32P4(const uint8* Crc_DataPtr, uint32 Crc_Length, uint32 Crc_StartValue32,
                            boolean Crc_IsFirstCall) {
  uint32_t crc = (Crc_IsFirstCall != static_cast<boolean>(0)) ? 0xFFFFFFFFu : Crc_StartValue32;
  if (Crc_DataPtr == NULL_PTR && Crc_Length != 0u) {
    return 0u;
  }
  for (uint32_t index = 0u; index < Crc_Length; ++index) {
    crc = Crc32Reflect(crc ^ Crc_DataPtr[index]);
    uint32_t x = crc;
    for (uint32_t bit = 0u; bit < 32u; ++bit) {
      if ((x & 0x80000000u) != 0u) {
        x = (x << 1u) ^ 0x1EDC6F41u;
      } else {
        x = x << 1u;
      }
    }
    crc = Crc32Reflect(x);
  }
  return static_cast<uint32>(crc ^ 0xFFFFFFFFu);
}

uint64 Crc_CalculateCRC64(const uint8* Crc_DataPtr, uint32 Crc_Length, uint64 Crc_StartValue64,
                          boolean Crc_IsFirstCall) {
  uint64_t crc = (Crc_IsFirstCall != static_cast<boolean>(0)) ? ~static_cast<uint64_t>(0u)
                                                                  : Crc_StartValue64;
  if (Crc_DataPtr == NULL_PTR && Crc_Length != 0u) {
    return 0u;
  }
  for (uint32_t index = 0u; index < Crc_Length; ++index) {
    crc = Crc64Reflect(crc ^ static_cast<uint64_t>(Crc_DataPtr[index]));
    uint64_t x = crc;
    for (uint32_t bit = 0u; bit < 64u; ++bit) {
      if ((x & 0x8000000000000000u) != 0u) {
        x = (x << 1u) ^ 0x42F0E1EBA9EA3693u;
      } else {
        x = x << 1u;
      }
    }
    crc = Crc64Reflect(x);
  }
  return static_cast<uint64>(crc ^ ~static_cast<uint64_t>(0u));
}

void Crc_GetVersionInfo(Std_VersionInfoType* Versioninfo) {
  if (Versioninfo != NULL_PTR) {
    Versioninfo->vendorID = 0x0000u;
    Versioninfo->moduleID = 0x00C9u;
    Versioninfo->sw_major_version = 0x04u;
    Versioninfo->sw_minor_version = 0x0Au;
    Versioninfo->sw_patch_version = 0x00u;
  }
}

}  // extern "C"