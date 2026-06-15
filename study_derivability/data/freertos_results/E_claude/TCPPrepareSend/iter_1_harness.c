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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_Transmission.h"

/* CBMC includes. */
#include "cbmc.h"
#include "../../utility/memory_assignments.c"

/* Abstraction of uxIPHeaderSizePacket. This test case only uses to test IPv4 path. */
size_t uxIPHeaderSizePacket( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return ipSIZE_OF_IPv4_HEADER;
}

/* Abstraction of uxIPHeaderSizeSocket. This test case only uses to test IPv4 path. */
size_t uxIPHeaderSizeSocket( const FreeRTOS_Socket_t * pxSocket )
{
    return ipSIZE_OF_IPv4_HEADER;
}

/* Abstraction of pxGetNetworkBufferWithDescriptor. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) safeMalloc( xRequestedSizeBytes );
        pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
        pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    return pxNetworkBuffer;
}

/* Abstraction of vReleaseNetworkBufferAndDescriptor. */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "FreeRTOS precondition: pxNetworkBuffer != NULL" );
}

/* Abstraction of prvTCPReturnPacket. */
void prvTCPReturnPacket( FreeRTOS_Socket_t * pxSocket,
                         NetworkBufferDescriptor_t * pxDescriptor,
                         uint32_t ulLen,
                         BaseType_t xReleaseAfterSend )
{
}

/* Abstraction of prvTCPSendPacket. */
int32_t prvTCPSendPacket( FreeRTOS_Socket_t * pxSocket )
{
    int32_t lReturn;

    return lReturn;
}

/* Abstraction of prvTCPSendRepeated. */
int32_t prvTCPSendRepeated( FreeRTOS_Socket_t * pxSocket,
                             NetworkBufferDescriptor_t ** ppxNetworkBuffer )
{
    int32_t lReturn;

    return lReturn;
}

/* Abstraction of xTCPWindowTxHasData. */
BaseType_t xTCPWindowTxHasData( TCPWindow_t const * pxWindow,
                                 uint32_t ulWindowSize,
                                 TickType_t * pulDelay )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of xTCPWindowTxDone. */
BaseType_t xTCPWindowTxDone( const TCPWindow_t * pxWindow )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of prvTCPPrepareSend. */
int32_t prvTCPPrepareSend( FreeRTOS_Socket_t * pxSocket,
                            NetworkBufferDescriptor_t ** ppxNetworkBuffer,
                            UBaseType_t uxOptionsLength )
{
    int32_t lReturn;

    return lReturn;
}

/* Abstraction of lTCPAddRxdata. */
BaseType_t lTCPAddRxdata( FreeRTOS_Socket_t * pxSocket,
                           size_t uxOffset,
                           const uint8_t * pcData,
                           uint32_t ulByteCount )
{
    BaseType_t xReturn;

    return xReturn;
}

/* Abstraction of xSendEventStructToIPTask. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                      TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of FreeRTOS_inet_ntop. */
const char * FreeRTOS_inet_ntop( BaseType_t xAddressFamily,
                                  const void * pvSource,
                                  char * pcDestination,
                                  socklen_t uxSize )
{
    return pcDestination;
}

void TCPPrepareSend_harness()
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xDataLength;
    UBaseType_t uxOptionsLength;

    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* Socket must be non-NULL for this function. */
    __CPROVER_assume( pxSocket != NULL );

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    /* Network buffer must be non-NULL. */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* The length of buffer must satisfy TCP minimum requirements. */
    __CPROVER_assume( xDataLength >= sizeof( TCPPacket_t ) && xDataLength <= ipconfigNETWORK_MTU );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) safeMalloc( xDataLength );

    /* Ethernet buffer must be non-NULL. */
    __CPR