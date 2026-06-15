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
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_Sockets.h"

/* CBMC includes. */
#include "cbmc.h"

/* The maximum size of a DHCP message that we might process. */
#ifndef DHCP_MAX_PACKET_SIZE
    #define DHCP_MAX_PACKET_SIZE    ( 1500U )
#endif

/* The minimum size of a DHCP message. */
#ifndef DHCP_MIN_PACKET_SIZE
    #define DHCP_MIN_PACKET_SIZE    ( sizeof( DHCPMessage_IPv4_t ) )
#endif

/* Global network endpoint pointer used by the DHCP process. */
extern NetworkEndPoint_t * pxNetworkEndPoints;

/* External DHCP socket. */
extern Socket_t xDHCPSocket;

/* Stub functions needed for CBMC analysis. */

BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    *pulNumber = nondet_uint32_t();
    return nondet_BaseType_t();
}

uint32_t ulApplicationTimeHook( void )
{
    return nondet_uint32_t();
}

TickType_t xTaskGetTickCount( void )
{
    return nondet_TickType_t();
}

void vTaskDelay( TickType_t xTicksToDelay )
{
    /* Stub: do nothing. */
}

BaseType_t xTaskResumeAll( void )
{
    return nondet_BaseType_t();
}

void vTaskSuspendAll( void )
{
    /* Stub: do nothing. */
}

void * pvPortMalloc( size_t xSize )
{
    return malloc( xSize );
}

void vPortFree( void * pv )
{
    free( pv );
}

/*
 * Stub for FreeRTOS_recvfrom.
 * Nondeterministically return data representing a DHCP response.
 */
int32_t FreeRTOS_recvfrom( Socket_t xSocket,
                            void * pvBuffer,
                            size_t uxBufferLength,
                            BaseType_t xFlags,
                            struct freertos_sockaddr * pxSourceAddress,
                            socklen_t * pxSourceAddressLength )
{
    int32_t lReturn = nondet_int32_t();

    if( lReturn > 0 )
    {
        /* Ensure return value is within valid range. */
        __CPROVER_assume( lReturn <= ( int32_t ) uxBufferLength );
        __CPROVER_assume( lReturn >= ( int32_t ) DHCP_MIN_PACKET_SIZE );

        if( pvBuffer != NULL )
        {
            /* Fill the buffer with nondeterministic data. */
            __CPROVER_havoc_object( pvBuffer );
        }
    }

    return lReturn;
}

/*
 * Stub for FreeRTOS_sendto.
 */
int32_t FreeRTOS_sendto( Socket_t xSocket,
                         const void * pvBuffer,
                         size_t uxTotalDataLength,
                         BaseType_t xFlags,
                         const struct freertos_sockaddr * pxDestinationAddress,
                         socklen_t xDestinationAddressLength )
{
    return nondet_int32_t();
}

/*
 * Stub for FreeRTOS_socket.
 */
Socket_t FreeRTOS_socket( BaseType_t xDomain,
                          BaseType_t xType,
                          BaseType_t xProtocol )
{
    Socket_t xSocket = nondet_voidp();
    return xSocket;
}

/*
 * Stub for FreeRTOS_setsockopt.
 */
BaseType_t FreeRTOS_setsockopt( Socket_t xSocket,
                                int32_t lLevel,
                                int32_t lOptionName,
                                const void * pvOptionValue,
                                size_t uxOptionLength )
{
    return nondet_BaseType_t();
}

/*
 * Stub for FreeRTOS_bind.
 */
BaseType_t FreeRTOS_bind( Socket_t xSocket,
                          struct freertos_sockaddr * pxAddress,
                          socklen_t xAddressLength )
{
    return nondet_BaseType_t();
}

/*
 * Stub for FreeRTOS_closesocket.
 */
BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
    return nondet_BaseType_t();
}

/*
 * Stub for FreeRTOS_IsNetworkUp.
 */
BaseType_t FreeRTOS_IsNetworkUp( void )
{
    return nondet_BaseType_t();
}

/*
 * Stub for vIPSetDHCPTimerEnableState.
 */
void vIPSetDHCPTimerEnableState( BaseType_t xEnableState )
{
    /* Stub: do nothing. */
}

/*
 * Stub for vDHCPTimerReload.
 */
void vDHCPTimerReload( TickType_t xLeaseTime )
{
    /* Stub: do nothing. */
}

/*
 * Stub for vIPReloadDHCPTimer.
 */
void vIPReloadDHCPTimer( uint32_t ulLeaseTime )
{
    /* Stub: do nothing. */
}

/*
 * Stub for xSendEventStructToIPTask.
 */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t xTimeout )
{
    return nondet_BaseType_t();
}

/*
 * Stub for xSendEventToIPTask.
 */
BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    return nondet_BaseType_t();
}

/*
 * Stub for vARPRefreshCacheEntry.
 */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIP