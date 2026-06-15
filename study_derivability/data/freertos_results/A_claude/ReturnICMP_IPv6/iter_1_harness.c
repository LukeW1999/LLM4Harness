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
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv6.h"
#include "FreeRTOS_ICMP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Routing.h"

/* CBMC proof includes. */
#include "cbmc.h"

/* The function under test. */
extern eFrameProcessingResult_t ReturnICMP_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                  size_t uxICMPSize );

/* Network buffer size constraints. */
#define TEST_MIN_BUFFER_SIZE    ( sizeof( EthernetHeader_t ) + sizeof( IPHeader_IPv6_t ) + sizeof( ICMPHeader_IPv6_t ) )
#define TEST_MAX_BUFFER_SIZE    ( ipconfigNETWORK_MTU + sizeof( EthernetHeader_t ) )

/* Helper to allocate a network endpoint nondeterministically. */
static NetworkEndPoint_t * prvGetEndpoint( void )
{
    NetworkEndPoint_t * pxEndPoint = NULL;

    if( nondet_bool() )
    {
        pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

        if( pxEndPoint != NULL )
        {
            __CPROVER_assume( pxEndPoint != NULL );
            memset( pxEndPoint, 0, sizeof( NetworkEndPoint_t ) );

            /* Set nondeterministic IPv6 addresses. */
            __CPROVER_havoc_object( &pxEndPoint->ipv6_settings );

            pxEndPoint->bits.bIPv6 = pdTRUE_UNSIGNED;
            pxEndPoint->pxNetworkInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );

            if( pxEndPoint->pxNetworkInterface != NULL )
            {
                memset( pxEndPoint->pxNetworkInterface, 0, sizeof( NetworkInterface_t ) );
            }
        }
    }

    return pxEndPoint;
}

/* Helper to allocate a network buffer with a valid packet buffer. */
static NetworkBufferDescriptor_t * prvGetNetworkBuffer( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xBufferSize;

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        memset( pxNetworkBuffer, 0, sizeof( NetworkBufferDescriptor_t ) );

        /* Allocate a buffer of nondeterministic but bounded size. */
        xBufferSize = nondet_size_t();
        __CPROVER_assume( xBufferSize >= TEST_MIN_BUFFER_SIZE );
        __CPROVER_assume( xBufferSize <= TEST_MAX_BUFFER_SIZE );

        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
        __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

        pxNetworkBuffer->xDataLength = xBufferSize;

        /* Assign a nondeterministic endpoint. */
        pxNetworkBuffer->pxEndPoint = prvGetEndpoint();

        /* Assign a nondeterministic interface. */
        if( nondet_bool() )
        {
            pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );

            if( pxNetworkBuffer->pxInterface != NULL )
            {
                memset( pxNetworkBuffer->pxInterface, 0, sizeof( NetworkInterface_t ) );
            }
        }
        else
        {
            pxNetworkBuffer->pxInterface = NULL;
        }
    }

    return pxNetworkBuffer;
}

void ReturnICMP_IPv6_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxICMPSize;
    eFrameProcessingResult_t eResult;

    /* Allocate a nondeterministic network buffer. */
    pxNetworkBuffer = prvGetNetworkBuffer();

    /* The function requires a non-NULL network buffer. */
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* The ICMPSize must be within valid bounds. */
    uxICMPSize = nondet_size_t();
    __CPROVER_assume( uxICMPSize >= sizeof( ICMPHeader_IPv6_t ) );
    __CPROVER_assume( uxICMPSize <= pxNetworkBuffer->xDataLength );

    /* Ensure the buffer is large enough to contain Ethernet + IPv6 + ICMP headers. */
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= TEST_MIN_BUFFER_SIZE );

    /* Havoc the buffer content to represent nondeterministic packet data. */
    __CPROVER_havoc_slice( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength );

    /* Call the function under test. */
    eResult = ReturnICMP_IPv6( pxNetworkBuffer, uxICMPSize );

    /* Postconditions:
     * The result must be one of the valid eFrameProcessingResult_t values.
     * Specifically, ReturnICMP_IPv6 is expected to return eReleaseBuffer or
     * eReturnEthernetFrame upon completion.
     */
    assert( ( eResult == eReleaseBuffer ) ||
            ( eResult == eReturnEthernetFrame ) ||
            ( eResult == eProcessBuffer ) ||
            ( eResult == eFrameConsumed ) ||
            ( eResult == eWaitingARPResolution ) );
}