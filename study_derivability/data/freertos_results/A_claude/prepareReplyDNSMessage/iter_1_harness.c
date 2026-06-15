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
#include <string.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Networking.h"
#include "FreeRTOS_DNS_Parser.h"

/* The maximum size of a DNS name. */
#ifndef ipconfigDNS_CACHE_NAME_LENGTH
    #define ipconfigDNS_CACHE_NAME_LENGTH    ( 254U )
#endif

/* Maximum payload length for a DNS packet. */
#define DNS_BUFFER_SIZE    ( ipconfigNETWORK_MTU )

/*
 * Stub for pvPortMalloc.
 */
void * pvPortMalloc( size_t xSize )
{
    return malloc( xSize );
}

/*
 * Stub for vPortFree.
 */
void vPortFree( void * pv )
{
    free( pv );
}

/*
 * Stub for FreeRTOS_ReleaseUDPPayloadBuffer.
 */
void FreeRTOS_ReleaseUDPPayloadBuffer( void const * pvBuffer )
{
    /* Nothing to do here in the stub. */
}

/*
 * Stub for FreeRTOS_GetUDPPayloadBuffer.
 */
void * FreeRTOS_GetUDPPayloadBuffer_Multi( size_t uxRequestedSizeBytes,
                                           TickType_t uxBlockTimeTicks,
                                           uint8_t ucIPType )
{
    void * pBuffer = NULL;

    if( nondet_bool() )
    {
        pBuffer = malloc( uxRequestedSizeBytes );
    }

    return pBuffer;
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
    Socket_t xSocket;

    if( nondet_bool() )
    {
        xSocket = FREERTOS_INVALID_SOCKET;
    }
    else
    {
        xSocket = malloc( sizeof( FreeRTOS_Socket_t ) );
    }

    return xSocket;
}

/*
 * Stub for FreeRTOS_bind.
 */
BaseType_t FreeRTOS_bind( Socket_t xSocket,
                          struct freertos_sockaddr const * pxAddress,
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
 * The harness for prepareReplyDNSMessage.
 */
void prepareReplyDNSMessage_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    BaseType_t lNetLength;

    /* Allocate a network buffer descriptor nondeterministically. */
    pxNetworkBuffer = malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate payload buffer for the network buffer. */
    pxNetworkBuffer->xDataLength = nondet_size_t();

    /* Constrain the data length to be within a reasonable range.
     * It must be large enough to hold at minimum the Ethernet + IP + UDP
     * headers plus the DNS header structure. */
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( UDPPacket_t ) + sizeof( DNSMessage_t ) );
    __CPROVER_assume( pxNetworkBuffer->xDataLength <= DNS_BUFFER_SIZE );

    pxNetworkBuffer->pucEthernetBuffer = malloc( pxNetworkBuffer->xDataLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* Set nondeterministic values in the network buffer's address fields. */
    pxNetworkBuffer->usPort = nondet_uint16_t();
    pxNetworkBuffer->usBoundPort = nondet_uint16_t();
    pxNetworkBuffer->ulIPAddress = nondet_uint32_t();

    /* Constrain lNetLength: it represents the length of the DNS reply payload.
     * Must be non-negative and within the buffer bounds. */
    lNetLength = nondet_BaseType_t();
    __CPROVER_assume( lNetLength >= 0 );
    __CPROVER_assume( ( size_t ) lNetLength <= pxNetworkBuffer->xDataLength );

    /* Call the function under test. */
    prepareReplyDNSMessage( pxNetworkBuffer, lNetLength );

    /* Postconditions:
     * The function should not have corrupted the network buffer pointer. */
    assert( pxNetworkBuffer != NULL );
    assert( pxNetworkBuffer->pucEthernetBuffer != NULL );
}