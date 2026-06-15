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

#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_Sockets.h"

/* CBMC proof helpers */
#include "cbmc.h"

/* External function under test */
extern void DHCPProcessEndPoint( BaseType_t xReset,
                                  BaseType_t xDoCheck,
                                  NetworkEndPoint_t * pxEndPoint );

/* Stub/model functions needed for verification */

NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxBuffer = NULL;

    if( nondet_bool() )
    {
        pxBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

        if( pxBuffer != NULL )
        {
            pxBuffer->xDataLength = xRequestedSizeBytes;
            pxBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xRequestedSizeBytes );

            if( pxBuffer->pucEthernetBuffer == NULL )
            {
                free( pxBuffer );
                pxBuffer = NULL;
            }
        }
    }

    return pxBuffer;
}

void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    if( pxNetworkBuffer != NULL )
    {
        if( pxNetworkBuffer->pucEthernetBuffer != NULL )
        {
            free( pxNetworkBuffer->pucEthernetBuffer );
        }

        free( pxNetworkBuffer );
    }
}

Socket_t FreeRTOS_socket( BaseType_t xDomain,
                           BaseType_t xType,
                           BaseType_t xProtocol )
{
    Socket_t xSocket = NULL;

    if( nondet_bool() )
    {
        xSocket = ( Socket_t ) malloc( sizeof( FreeRTOS_Socket_t ) );
    }

    return xSocket;
}

BaseType_t FreeRTOS_setsockopt( Socket_t xSocket,
                                 int32_t lLevel,
                                 int32_t lOptionName,
                                 const void * pvOptionValue,
                                 size_t uxOptionLength )
{
    return nondet_BaseType_t();
}

BaseType_t FreeRTOS_bind( Socket_t xSocket,
                           struct freertos_sockaddr * pxAddress,
                           socklen_t xAddressLength )
{
    return nondet_BaseType_t();
}

BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
    if( xSocket != NULL )
    {
        free( xSocket );
    }

    return pdPASS;
}

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

    if( lReturn > 0 )
    {
        /* Fill buffer with nondeterministic data */
        __CPROVER_havoc_slice( pvBuffer, lReturn );
    }

    return lReturn;
}

int32_t FreeRTOS_sendto( Socket_t xSocket,
                          const void * pvBuffer,
                          size_t uxTotalDataLength,
                          BaseType_t xFlags,
                          const struct freertos_sockaddr * pxDestinationAddress,
                          socklen_t xDestinationAddressLength )
{
    return nondet_int32_t();
}

TickType_t xTaskGetTickCount( void )
{
    return nondet_TickType_t();
}

void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
    __CPROVER_assert( pxTimeOut != NULL, "pxTimeOut is not NULL" );
}

BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut,
                                  TickType_t * const pxTicksToWait )
{
    return nondet_BaseType_t();
}

BaseType_t xSocketValid( const ConstSocket_t xSocket )
{
    return nondet_BaseType_t();
}

void * pvPortMalloc( size_t xWantedSize )
{
    return malloc( xWantedSize );
}

void vPortFree( void * pv )
{
    free( pv );
}

BaseType_t xSendDHCPEvent( struct xNetworkEndPoint * pxEndPoint )
{
    return nondet_BaseType_t();
}

void vIPSetDHCPTimerEnableState( NetworkEndPoint_t * pxEndPoint,
                                  BaseType_t xEnableState )
{
    ( void ) pxEndPoint;
    ( void ) xEnableState;
}

void vDHCPTimerReload( NetworkEndPoint_t * pxEndPoint,
                        TickType_t uxClockTicks )
{
    ( void ) pxEndPoint;
    ( void ) uxClockTicks;
}

void vIPNetworkUpCalls( struct xNetworkEndPoint * pxEndPoint )
{
    ( void ) pxEndPoint;
}

BaseType_t xApplicationDHCPHook_Multi( eDHCPCallbackPhase_t eDHCPPhase,
                                        struct xNetworkEndPoint * pxEndPoint,
                                        IP_Address_t * pxIPAddress )
{
    return nondet_BaseType_t();
}

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                              uint16_t usSourcePort,
                                              uint32_t ulDestinationAddress,
                                              uint16_t usDestinationPort )
{
    return nondet_uint32_t();
}

BaseType_t xNetworkInterfaceOutput( struct x