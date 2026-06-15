#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "memory_assignments.c"

uint16_t prvGetPrivatePortNumber( BaseType_t xProtocol )
{
    uint16_t usResult;
    return usResult;
}

void vSocketBind_harness()
{
    Socket_t xSocket;
    struct freertos_sockaddr *pxAddress;
    socklen_t xAddressLength;

    xSocket = ( Socket_t ) nondet_pointer();
    pxAddress = ( struct freertos_sockaddr * ) nondet_pointer();
    xAddressLength = nondet_uint16();

    __CPROVER_assume( pxAddress != NULL );
    __CPROVER_assume( xAddressLength >= sizeof( struct freertos_sockaddr ) );

    vSocketBind( xSocket, pxAddress, xAddressLength );

    /* Postconditions */
    if ( xSocket != FREERTOS_INVALID_SOCKET )
    {
        assert( xSocket->u.xTCP.usLocalPort == pxAddress->sin_port );
    }
}