/* AUTOETH Classic C compatibility: TcpIp host imports.
 * These are explicit LOCAL host mappings (R0.4 topic "host imports"), not a
 * claim about the full TcpIp binary ABI. */
#ifndef AUTOETH_CLASSIC_TCPIP_TYPES_H
#define AUTOETH_CLASSIC_TCPIP_TYPES_H

#include "autoeth/classic/Std_Types.h"

typedef uint32 TcpIp_SocketIdType;
typedef uint16 TcpIp_LocalAddrIdType;
typedef uint16 TcpIp_DomainType;

#define TCPIP_AF_INET ((TcpIp_DomainType)2u)
#define TCPIP_AF_INET6 ((TcpIp_DomainType)28u)

typedef uint8 TcpIp_ReturnType;
#define TCPIP_E_OK ((TcpIp_ReturnType)0u)
#define TCPIP_E_NOT_OK ((TcpIp_ReturnType)1u)
#define TCPIP_E_PENDING ((TcpIp_ReturnType)2u)

typedef uint8 TcpIp_IpAddrStateType;
#define TCPIP_ASSIGNED ((TcpIp_IpAddrStateType)0u)
#define TCPIP_ONHOLD ((TcpIp_IpAddrStateType)1u)
#define TCPIP_UNASSIGNED ((TcpIp_IpAddrStateType)2u)

typedef uint8 TcpIp_EventType;
#define TCPIP_TCP_RESET ((TcpIp_EventType)0u)
#define TCPIP_TCP_CLOSED ((TcpIp_EventType)1u)
#define TCPIP_TCP_FIN_RECEIVED ((TcpIp_EventType)2u)
#define TCPIP_UDP_CLOSED ((TcpIp_EventType)3u)

#define AUTOETH_TCPIP_ADDR_BYTES 16u

typedef struct TcpIp_SockAddrType_tag {
  TcpIp_DomainType domain;
  uint16 port;
  uint8 addr[AUTOETH_TCPIP_ADDR_BYTES];
  uint32 scope_id;
} TcpIp_SockAddrType;

#endif