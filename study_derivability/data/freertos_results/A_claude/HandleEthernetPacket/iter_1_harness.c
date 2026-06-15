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
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"
#include "FreeRTOS_ARP.h"

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
void HandleEthernetPacket( NetworkBufferDescriptor_t * pxNetworkBuffer );

/*
 * Stub for vReleaseNetworkBufferAndDescriptor to avoid memory issues
 * during verification.
 */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    /* Do nothing - stub for proof. */
}

/*
 * Stub for eConsiderFrameForProcessing.
 */
eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer )
{
    eFrameProcessingResult_t eResult;
    eResult = nondet_uint32();
    __CPROVER_assume( eResult == eProcessBuffer ||
                      eResult == eReleaseBuffer ||
                      eResult == eReturnEthernetFrame ||
                      eResult == eFrameConsumed );
    return eResult;
}

/*
 * Stub for xSendEventStructToIPTask.
 */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    return nondet_BaseType_t();
}

/*
 * Allocate and initialize a NetworkBufferDescriptor_t for use in the proof.
 */
static NetworkBufferDescriptor_t * prvAllocateNetworkBuffer( void )
{
    NetworkBufferDescriptor_t * pxBuffer;
    size_t uxBufferLength;
    uint8_t * pucBuffer;

    pxBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxBuffer != NULL )
    {
        /* Choose a nondeterministic length for the Ethernet frame,
         * but ensure it is at least large enough to hold an Ethernet header. */
        uxBufferLength = nondet_uint32();
        __CPROVER_assume( uxBufferLength >= sizeof( EthernetHeader_t ) );
        __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

        pucBuffer = ( uint8_t * ) malloc( uxBufferLength );
        __CPROVER_assume( pucBuffer != NULL );

        pxBuffer->pucEthernetBuffer = pucBuffer;
        pxBuffer->xDataLength = uxBufferLength;

        /* Initialize the interface pointer nondeterministically. */
        NetworkInterface_t * pxInterface = malloc( sizeof( NetworkInterface_t ) );
        pxBuffer->pxInterface = pxInterface;

        /* Initialize the endpoint pointer nondeterministically. */
        NetworkEndPoint_t * pxEndPoint = malloc( sizeof( NetworkEndPoint_t ) );
        pxBuffer->pxEndPoint = pxEndPoint;
    }

    return pxBuffer;
}

void HandleEthernetPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;

    /* Allocate and initialize the network buffer nondeterministically. */
    pxNetworkBuffer = prvAllocateNetworkBuffer();

    /* The function must receive a valid (non-NULL) buffer pointer,
     * as per the precondition documented in the FreeRTOS+TCP source. */
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( EthernetHeader_t ) );

    /* Call the function under test. */
    HandleEthernetPacket( pxNetworkBuffer );

    /* No postconditions to assert explicitly for this function since
     * it returns void and manages its own buffer lifecycle internally.
     * Memory safety is verified by CBMC automatically. */
}