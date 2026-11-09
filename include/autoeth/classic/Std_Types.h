/* AUTOETH Classic C compatibility: Std_Types.h
 * Std_ReturnType and Std_VersionInfoType keep the source order
 * (vendorID, moduleID, sw_major/minor/patch). Includes Platform_Types.h (R0.1.1). */
#ifndef AUTOETH_CLASSIC_STD_TYPES_H
#define AUTOETH_CLASSIC_STD_TYPES_H

#include "autoeth/classic/Platform_Types.h"

typedef uint8 Std_ReturnType;

#define E_OK ((Std_ReturnType)0x00u)
#define E_NOT_OK ((Std_ReturnType)0x01u)

#define STD_HIGH ((uint8)0x01u)
#define STD_LOW ((uint8)0x00u)

#define STD_ACTIVE ((uint8)0x01u)
#define STD_IDLE ((uint8)0x00u)

#define STD_ON ((uint8)0x01u)
#define STD_OFF ((uint8)0x00u)

typedef struct Std_VersionInfoType_tag {
  uint16 vendorID;
  uint16 moduleID;
  uint8 sw_major_version;
  uint8 sw_minor_version;
  uint8 sw_patch_version;
} Std_VersionInfoType;

typedef uint8 Std_MessageTypeType;
typedef uint8 Std_MessageResultType;

#define STD_MESSAGETYPE_REQUEST ((Std_MessageTypeType)0x00u)
#define STD_MESSAGETYPE_RESPONSE ((Std_MessageTypeType)0x01u)

#define STD_MESSAGERESULT_OK ((Std_MessageResultType)0x00u)
#define STD_MESSAGERESULT_ERROR ((Std_MessageResultType)0x01u)

#endif