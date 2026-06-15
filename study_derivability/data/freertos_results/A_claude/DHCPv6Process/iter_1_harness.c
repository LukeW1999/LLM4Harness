```c
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
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DHCPv6.h"

/* CBMC proof utility includes. */
#include "cbmc.h"

/* ======================== Stub definitions ======================== */

/* Global variables used in stubs/proof. */
NetworkEndPoint_t * pxNetworkEndPoint_Temp;
BaseType_t xDHCPv6Socket_created;

/* Stub for xTaskGetTickCount */
TickType_t xTaskGetTickCount( void )
{
    return ( TickType_t ) nondet_uint32_t();
}

/* Stub for FreeRTOS_socket */
Socket_t FreeRTOS_socket( BaseType_t xDomain,
                          BaseType_t xType,
                          BaseType_t xProtocol )
{
    Socket_t xSocket;

    if( nondet_bool() )
    {
        xSocket = FREERTOS_INVALID_SOCKET;
    }
    else
    {
        xSocket = ( Socket_t ) malloc( sizeof( FreeRTOS_Socket_t ) );
        __CPROVER_assume( xSocket != NULL );
        __CPROVER_assume( xSocket != FREERTOS_INVALID_SOCKET );
    }

    return xSocket;
}

/* Stub for FreeRTOS_setsockopt */
BaseType_t FreeRTOS_setsockopt( Socket_t xSocket,
                                int32_t lLevel,
                                int32_t lOptionName,
                                const void * pvOptionValue,
                                size_t uxOptionLength )
{
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for FreeRTOS_bind */
BaseType_t FreeRTOS_bind( Socket_t xSocket,
                          struct freertos_sockaddr * pxAddress,
                          socklen_t xAddressLength )
{
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for FreeRTOS_closesocket */
BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for FreeRTOS_recvfrom */
int32_t FreeRTOS_recvfrom( Socket_t xSocket,
                           void * pvBuffer,
                           size_t uxBufferLength,
                           BaseType_t xFlags,
                           struct freertos_sockaddr * pxSourceAddress,
                           socklen_t * pxSourceAddressLength )
{
    int32_t lReturn = nondet_int32_t();
    __CPROVER_assume( lReturn >= -1 );
    __CPROVER_assume( lReturn <= ( int32_t ) uxBufferLength );
    return lReturn;
}

/* Stub for FreeRTOS_sendto */
int32_t FreeRTOS_sendto( Socket_t xSocket,
                         const void * pvBuffer,
                         size_t uxTotalDataLength,
                         BaseType_t xFlags,
                         const struct freertos_sockaddr * pxDestinationAddress,
                         socklen_t xDestinationAddressLength )
{
    return ( int32_t ) nondet_int32_t();
}

/* Stub for vDHCPv6ProcessEndPoint */
void vDHCPv6ProcessEndPoint( BaseType_t xReset,
                             NetworkEndPoint_t * pxEndPoint,
                             DHCPMessage_IPv6_t * pxDHCPMessage )
{
    /* Nondeterministic stub - may modify endpoint state */
    __CPROVER_assert( pxEndPoint != NULL, "pxEndPoint must not be NULL" );
}

/* Stub for xSocketValid */
BaseType_t xSocketValid( const ConstSocket_t xSocket )
{
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for FreeRTOS_inet_pton6 */
BaseType_t FreeRTOS_inet_pton6( const char * pcSource,
                                void * pvDestination )
{
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for vIPSetDHCP_RATimerEnableState */
void vIPSetDHCP_RATimerEnableState( NetworkEndPoint_t * pxEndPoint,
                                   BaseType_t xEnableState )
{
}

/* Stub for vIPReloadDHCP_RATimer */
void vIPReloadDHCP_RATimer( NetworkEndPoint_t * pxEndPoint,
                            TickType_t uxClockTicks )
{
}

/* Stub for xApplicationGetRandomNumber */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    *pulNumber = nondet_uint32_t();
    return ( BaseType_t ) nondet_int32_t();
}

/* Stub for pcApplicationHostnameHook */
const char * pcApplicationHostnameHook( void )
{
    return "freertos";
}

/* Stub for FreeRTOS_GetAddressConfiguration */
void FreeRTOS_GetAddressConfiguration( uint32_t * pulIPAddress,
                                       uint32_t * pulNetMask,
                                       uint32_t * pulGatewayAddress,
                                       uint32_t * pulDNSServerAddress )
{
}

/* Stub for vDHCPv6Stop */
void vDHCPv6Stop( NetworkEndPoint_t * pxEndPoint )
{
}

/* ======================== Helper to allocate endpoint ======================== */

static NetworkEndPoint_t * allocate_and_init_endpoint( void )
{
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    __CPROVER_assume( pxEndPoint != NULL );

    /* Initialize with nondeterministic values */
    pxEndPoint->xDHCPData.eDHCPState    = ( eDHCPState_t ) nondet_uint32_t();
    pxEndPoint->xDHCPData.eExpectedState = ( eDHCPState_t ) nondet_uint32_t();
    px