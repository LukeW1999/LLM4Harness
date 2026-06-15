#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "cbmc.h"

extern DHCPMessage_IPv6_t xDHCPMessage;

BaseType_t DHCPv6Analyse( struct xNetworkEndPoint * pxEndPoint,
                         const uint8_t * pucAnswer,
                         size_t uxTotalLength );

void DHCPv6Analyse_harness()
{
    struct xNetworkEndPoint * pxEndPoint;
    const uint8_t * pucAnswer;
    size_t uxTotalLength;

    pxEndPoint = safeMalloc( sizeof( struct xNetworkEndPoint ) );
    pucAnswer = safeMalloc( uxTotalLength );

    __CPROVER_assume( pxEndPoint != NULL );
    __CPROVER_assume( pucAnswer != NULL );
    __CPROVER_assume( uxTotalLength > 0 && uxTotalLength <= 1500 );

    DHCPv6Analyse( pxEndPoint, pucAnswer, uxTotalLength );
}