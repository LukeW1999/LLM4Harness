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
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ND.h"

/* CBMC proof includes. */
#include "cbmc.h"

/* The function under test. */
void ReceiveRA( const NetworkBufferDescriptor_t * pxNetworkBuffer );

/* Maximum size for the network buffer payload. */
#ifndef TEST_MAX_BUFFER_SIZE
    #define TEST_MAX_BUFFER_SIZE    1500U
#endif

/* Stub for vDHCP_RATimerReload if needed. */

/* Proof harness. */
void ReceiveRA_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxBufferLength;
    uint8_t * pucEthernetBuffer;

    /* Allocate a NetworkBufferDescriptor_t nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* ReceiveRA requires a non-NULL network buffer. */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Choose a nondeterministic buffer length. Must be large enough to contain
     * at least an Ethernet header + IPv6 header + ICMPv6 RA header. */
    uxBufferLength = nondet_size_t();
    __CPROVER_assume( uxBufferLength >= sizeof( EthernetHeader_t ) +
                                        sizeof( IPHeader_IPv6_t ) +
                                        sizeof( ICMPRouterAdvertisement_IPv6_t ) );
    __CPROVER_assume( uxBufferLength <= TEST_MAX_BUFFER_SIZE );

    /* Allocate a nondeterministic Ethernet buffer for the network buffer. */
    pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Set the network buffer fields. */
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = uxBufferLength;

    /* The network interface pointer can be NULL or non-NULL depending on
     * code paths, set it nondeterministically. */
    NetworkInterface_t * pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    pxNetworkBuffer->pxInterface = pxInterface;

    /* If the interface is allocated, set its endpoint list nondeterministically. */
    if( pxInterface != NULL )
    {
        NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        pxInterface->pxEndPoint = pxEndPoint;

        if( pxEndPoint != NULL )
        {
            /* Initialise endpoint fields nondeterministically. */
            /* bits.bIPv6 indicates whether the endpoint uses IPv6. */
            pxEndPoint->bits.bIPv6 = nondet_BaseType_t() ? 1U : 0U;

            /* Set the RA data within the endpoint. */
            pxEndPoint->xRAData.bits.bIPAddressInUse = nondet_BaseType_t() ? 1U : 0U;
            pxEndPoint->xRAData.bits.bRouterReplied = nondet_BaseType_t() ? 1U : 0U;
            pxEndPoint->xRAData.uxRetryCount = nondet_UBaseType_t();

            /* Set a valid IPv6 address for the endpoint. */
            /* The address can be left as nondeterministic bytes. */

            /* pxNext for endpoint linked list - set to NULL for simplicity. */
            pxEndPoint->pxNext = NULL;
        }
    }

    /* Also set the endpoint pointer directly on the network buffer. */
    NetworkEndPoint_t * pxEndPointDirect = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    pxNetworkBuffer->pxEndPoint = pxEndPointDirect;

    if( pxEndPointDirect != NULL )
    {
        pxEndPointDirect->bits.bIPv6 = nondet_BaseType_t() ? 1U : 0U;
        pxEndPointDirect->xRAData.bits.bIPAddressInUse = nondet_BaseType_t() ? 1U : 0U;
        pxEndPointDirect->xRAData.bits.bRouterReplied = nondet_BaseType_t() ? 1U : 0U;
        pxEndPointDirect->xRAData.uxRetryCount = nondet_UBaseType_t();
        pxEndPointDirect->pxNext = NULL;
        pxEndPointDirect->pxNetworkInterface = pxInterface;
    }

    /* Call the function under test. */
    ReceiveRA( pxNetworkBuffer );

    /* Postconditions:
     * The function does not return a value, so we assert memory safety
     * by checking that the buffer pointer remains valid (not corrupted).
     * The main safety property is that CBMC verifies no memory violations
     * occurred during execution.
     */
    assert( pxNetworkBuffer != NULL );
    assert( pxNetworkBuffer->pucEthernetBuffer == pucEthernetBuffer );
}