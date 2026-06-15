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

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv6.h"
#include "FreeRTOS_ND.h"

/* CBMC proof helpers */
#include "cbmc.h"

/* The size of the network buffer data must be large enough to hold an
 * ICMPv6 packet. We pick a reasonable upper bound for proof. */
#ifndef TEST_ICMP_PACKET_SIZE
    #define TEST_ICMP_PACKET_SIZE    ( ipconfigNETWORK_MTU )
#endif

/* Stub / model for xTaskGetTickCount if needed */

/**
 * @brief Allocate and initialise a NetworkBufferDescriptor_t with
 *        nondeterministic content for use in CBMC proofs.
 */
static NetworkBufferDescriptor_t * allocate_NetworkBuffer( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xDataLength;

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Choose a nondeterministic but bounded data length that is at least
     * large enough to contain the Ethernet + IPv6 + ICMPv6 headers. */
    __CPROVER_assume( xDataLength >= sizeof( EthernetHeader_t ) +
                                     sizeof( IPHeader_IPv6_t ) +
                                     sizeof( ICMPHeader_IPv6_t ) );
    __CPROVER_assume( xDataLength <= TEST_ICMP_PACKET_SIZE );

    pxNetworkBuffer->xDataLength = xDataLength;

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xDataLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* Leave the buffer contents nondeterministic – CBMC will explore all
     * possible byte values automatically. */

    /* Initialise the list item so list operations do not crash. */
    vListInitialiseItem( &( pxNetworkBuffer->xBufferListItem ) );

    /* Associate a nondeterministic (possibly NULL) network interface. */
    pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    /* pxInterface may be NULL – do not assume non-NULL */

    /* Associate a nondeterministic (possibly NULL) endpoint. */
    pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    if( pxNetworkBuffer->pxEndPoint != NULL )
    {
        /* Fill endpoint fields nondeterministically. */
        pxNetworkBuffer->pxEndPoint->pxNext = NULL;
    }

    return pxNetworkBuffer;
}

/**
 * @brief CBMC proof harness for ProcessICMPMessage_IPv6.
 */
void ProcessICMPMessage_IPv6_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    eFrameProcessingResult_t eResult;

    /* -----------------------------------------------------------------
     * Allocate and constrain the network buffer.
     * ----------------------------------------------------------------- */
    pxNetworkBuffer = allocate_NetworkBuffer();

    /* The function under test must receive a non-NULL network buffer. */
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* -----------------------------------------------------------------
     * Call the function under test.
     * ----------------------------------------------------------------- */
    eResult = ProcessICMPMessage_IPv6( pxNetworkBuffer );

    /* -----------------------------------------------------------------
     * Postconditions.
     *
     * The function must return one of the valid eFrameProcessingResult_t
     * values.  The exact set accepted depends on the FreeRTOS+TCP version;
     * we assert the most general property: the return value is within the
     * valid enum range.
     * ----------------------------------------------------------------- */
    assert( ( eResult == eReleaseBuffer ) ||
            ( eResult == eProcessBuffer ) ||
            ( eResult == eReturnEthernetFrame ) ||
            ( eResult == eFrameConsumed ) ||
            ( eResult == eWaitingARPResolution ) );
}