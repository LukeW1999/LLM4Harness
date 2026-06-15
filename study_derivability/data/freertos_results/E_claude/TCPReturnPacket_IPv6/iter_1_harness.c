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
#include "FreeRTOS_TCP_Transmission_IPv6.h"

/* CBMC includes. */
#include "cbmc.h"
#include "../../utility/memory_assignments.c"

/* Abstraction of uxIPHeaderSizePacket for IPv6. */
size_t uxIPHeaderSizePacket( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return ipSIZE_OF_IPv6_HEADER;
}

/* Abstraction of xIsCallingFromIPTask. */
BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnNetMask_IPv6. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask_IPv6( const IPv6_Address_t * pxIPv6Address )
{
    return ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
}

/* Abstraction of FreeRTOS_FindGateWay. */
NetworkEndPoint_t * FreeRTOS_FindGateWay( BaseType_t xIPType )
{
    return ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
}

/* Abstraction of eNDGetCacheEntry. */
eARPLookupResult_t eNDGetCacheEntry( IPv6_Address_t * pxIPAddress,
                                     MACAddress_t * const pxMACAddress,
                                     struct xNetworkEndPoint ** ppxEndPoint )
{
    eARPLookupResult_t eReturn;

    __CPROVER_assume( eReturn == eARPCacheHit ||
                      eReturn == eARPCacheMiss ||
                      eReturn == eCantSendPacket );

    if( ppxEndPoint != NULL )
    {
        *ppxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    if( pxMACAddress != NULL && eReturn == eARPCacheHit )
    {
        __CPROVER_havoc_object( pxMACAddress );
    }

    return eReturn;
}

/* Abstraction of vNDSendNeighbourSolicitation. */
void vNDSendNeighbourSolicitation( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                   const IPv6_Address_t * pxIPAddress )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "vNDSendNeighbourSolicitation: pxNetworkBuffer != NULL" );
    __CPROVER_assert( pxIPAddress != NULL,
                      "vNDSendNeighbourSolicitation: pxIPAddress != NULL" );
}

/* Abstraction of xNetworkInterfaceOutput. */
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                    BaseType_t xReleaseAfterSend )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "xNetworkInterfaceOutput: pxNetworkBuffer != NULL" );

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of vReleaseNetworkBufferAndDescriptor. */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "vReleaseNetworkBufferAndDescriptor: pxNetworkBuffer != NULL" );
}

/* Abstraction of xSendEventStructToIPTask. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxEvent != NULL,
                      "xSendEventStructToIPTask: pxEvent != NULL" );

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of pxGetNetworkBufferWithDescriptor. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxBuffer != NULL )
    {
        pxBuffer->pucEthernetBuffer = ( uint8_t * ) safeMalloc( xRequestedSizeBytes );
        pxBuffer->xDataLength = xRequestedSizeBytes;
        pxBuffer->pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    return pxBuffer;
}

/* Abstraction of uxIPHeaderSizeSocket for IPv6. */
size_t uxIPHeaderSizeSocket( const FreeRTOS_Socket_t * pxSocket )
{
    return ipSIZE_OF_IPv6_HEADER;
}

void TCPReturnPacket_IPv6_harness()
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint32_t ulLen;
    BaseType_t xReleaseAfterSend;
    size_t xDataLength;

    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    /* The length of buffer must be at least large enough for a TCP/IPv6 packet. */
    __CPROVER_assume( xDataLength >= sizeof( TCPPacket_