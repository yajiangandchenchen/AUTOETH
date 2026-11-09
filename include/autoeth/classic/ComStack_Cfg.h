/* AUTOETH Classic C compatibility: ComStack_Cfg.h
 * PduIdType is uint16 and PduLengthType is uint32 for this host (R0.2.1 topic
 * "ComStack and version structures"). No ARXML generator is implied. */
#ifndef AUTOETH_CLASSIC_COMSTACK_CFG_H
#define AUTOETH_CLASSIC_COMSTACK_CFG_H

#include "autoeth/classic/Platform_Types.h"

typedef uint16 PduIdType;
typedef uint32 PduLengthType;

#endif