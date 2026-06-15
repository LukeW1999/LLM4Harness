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

#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

#include "cbmc.h"

/* eConsiderFrameForProcessing is defined in FreeRTOS_ARP.c */
eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer );

/*
 * We need a global list of endpoints so that the function can iterate
 * over them when checking MAC addresses.
 */
extern NetworkEndPoint_t * pxNetworkEndPoints;

/*
 * Stub for FreeRTOS_FindEndPointOnMAC - proved separately.
 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    /* Return either NULL or a valid endpoint non-deterministically */
    NetworkEndPoint_t * pxEndPoint = NULL;

    if( nondet_bool() )
    {
        pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

        if( pxEndPoint != NULL )
        {
            /* Leave endpoint fields unconstrained */
            pxEndPoint->pxNext = NULL;
        }
    }

    return pxEndPoint;
}

void harness()
{
    /* Allocate a buffer large enough to hold an Ethernet frame header */
    uint8_t * pucEthernetBuffer;
    size_t xBufferSize;

    /* The minimum size needed is the Ethernet header: destination MAC (6),
     * source MAC (6), and EtherType (2) = 14 bytes = sizeof(EthernetHeader_t) */
    __CPROVER_assume( xBufferSize >= sizeof( EthernetHeader_t ) );
    __CPROVER_assume( xBufferSize <= ipconfigNETWORK_MTU );

    pucEthernetBuffer = ( uint8_t * ) safeMalloc( xBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Set up a global endpoint list so the function can walk it */
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    if( pxEndPoint != NULL )
    {
        /* Give it an unconstrained MAC address */
        /* pxEndPoint->xMACAddress is left non-deterministic */
        pxEndPoint->pxNext = NULL;
    }

    pxNetworkEndPoints = pxEndPoint;

    /* Call the function under verification */
    eFrameProcessingResult_t eResult = eConsiderFrameForProcessing( pucEthernetBuffer );

    /* The result must be one of the valid enum values */
    __CPROVER_assert(
        eResult == eReleaseBuffer ||
        eResult == eProcessBuffer ||
        eResult == eReturnEthernetFrame ||
        eResult == eFrameConsumed ||
        eResult == eWaitingARPResolution,
        "Result is a valid eFrameProcessingResult_t value" );
}