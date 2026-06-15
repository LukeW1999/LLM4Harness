/*
 * FreeRTOS memory safety proofs with CBMC.
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_DHCPv6.h"
#include "FreeRTOS_BitConfig.h"

/* CBMC includes. */
#include "cbmc.h"

/* ===== Mock / stub functions ===== */

/* Mock vDHCPv6ProcessEndPoint so we don't need to prove it here. */
void __CPROVER_file_local_FreeRTOS_DHCPv6_c_vDHCPv6ProcessEndPoint( BaseType_t xReset,
                                                                     NetworkEndPoint_t * pxEndPoint,
                                                                     DHCPMessage_IPv6_t * pxDHCPMessage )
{
    __CPROVER_assert( __CPROVER_r_ok( pxEndPoint, sizeof( NetworkEndPoint_t ) ),
                      "pxEndPoint must be readable in vDHCPv6ProcessEndPoint" );
}

/* Mock socket-related calls that may be exercised. */
Socket_t FreeRTOS_socket( BaseType_t xDomain,
                          BaseType_t xType,
                          BaseType_t xProtocol )
{
    /* Return either a valid non-NULL pointer or FREERTOS_INVALID_SOCKET. */
    Socket_t xSocket;

    if( nondet_bool() )
    {
        xSocket = safeMalloc( sizeof( *xSocket ) );
    }
    else
    {
        xSocket = FREERTOS_INVALID_SOCKET;
    }

    return xSocket;
}

BaseType_t FreeRTOS_setsockopt( Socket_t xSocket,
                                int32_t lLevel,
                                int32_t lOptionName,
                                const void * pvOptionValue,
                                size_t uxOptionLength )
{
    BaseType_t xReturn;
    return xReturn;
}

BaseType_t FreeRTOS_bind( Socket_t xSocket,
                          struct freertos_sockaddr * pxAddress,
                          socklen_t xAddressLength )
{
    BaseType_t xReturn;
    return xReturn;
}

BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
    BaseType_t xReturn;
    return xReturn;
}

int32_t FreeRTOS_recvfrom( Socket_t xSocket,
                           void * pvBuffer,
                           size_t uxBufferLength,
                           BaseType_t xFlags,
                           struct freertos_sockaddr * pxSourceAddress,
                           socklen_t * pxSourceAddressLength )
{
    int32_t lReturn;
    return lReturn;
}

int32_t FreeRTOS_sendto( Socket_t xSocket,
                         const void * pvBuffer,
                         size_t uxTotalDataLength,
                         BaseType_t xFlags,
                         const struct freertos_sockaddr * pxDestinationAddress,
                         socklen_t xDestinationAddressLength )
{
    int32_t lReturn;
    return lReturn;
}

/* Mock time / tick functions. */
TickType_t xTaskGetTickCount( void )
{
    TickType_t xTick;
    return xTick;
}

BaseType_t xTaskGetSchedulerState( void )
{
    return taskSCHEDULER_RUNNING;
}

/* Mock vIPSetDHCP_RATimerEnableState. */
void vIPSetDHCP_RATimerEnableState( NetworkEndPoint_t * pxEndPoint,
                                    BaseType_t xEnableState )
{
    __CPROVER_assert( __CPROVER_r_ok( pxEndPoint, sizeof( NetworkEndPoint_t ) ),
                      "pxEndPoint must be readable in vIPSetDHCP_RATimerEnableState" );
}

/* Mock vIPReloadDHCP_RATimer. */
void vIPReloadDHCP_RATimer( struct xNetworkEndPoint * pxEndPoint,
                            TickType_t uxClockTicks )
{
    __CPROVER_assert( __CPROVER_r_ok( pxEndPoint, sizeof( NetworkEndPoint_t ) ),
                      "pxEndPoint must be readable in vIPReloadDHCP_RATimer" );
}

/* Mock FreeRTOS_inet_pton6. */
BaseType_t FreeRTOS_inet_pton6( const char * pcSource,
                                void * pvDestination )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Mock prvDHCPv6Analyse - tested separately. */
BaseType_t __CPROVER_file_local_FreeRTOS_DHCPv6_c_prvDHCPv6Analyse( struct xNetworkEndPoint * pxEndPoint,
                                                                     const uint8_t * pucAnswer,
                                                                     size_t uxTotalLength,
                                                                     DHCPMessage_IPv6_t * pxDHCPMessage )
{
    BaseType_t xReturn;

    __CPROVER_assert( __CPROVER_r_ok( pxEndPoint, sizeof( NetworkEndPoint_t ) ),
                      "pxEndPoint must be readable in prvDHCPv6Analyse" );
    __CPROVER_assert( uxTotalLength == 0 || __CPROVER_r_ok( pucAnswer, uxTotalLength ),
                      "pucAnswer region must be readable in prvDHCPv6Analyse" );
    __CPROVER_assert( __CPROVER_r_ok( pxDHCPMessage, sizeof( DHCPMessage_IPv6_t ) ),
                      "pxDHCPMessage must be readable in prvDHCPv6Analyse" );

    return xReturn;
}

/* ===== Harness ===== */

void DHCPv6Process_harness()
{
    BaseType_t xReset;
    NetworkEndPoint_t * pxEndPoint;

    /* Allocate