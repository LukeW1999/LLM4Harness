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
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"

#include "cbmc.h"

/* Abstraction of xQueueGenericSend */
BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
    BaseType_t xReturn;

    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of vReleaseNetworkBufferAndDescriptor */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "vReleaseNetworkBufferAndDescriptor: pxNetworkBuffer != NULL" );
}

/* Abstraction of xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of eConsiderFrameForProcessing */
eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer )
{
    eFrameProcessingResult_t eReturn;

    __CPROVER_assert( pucEthernetBuffer != NULL,
                      "eConsiderFrameForProcessing: pucEthernetBuffer != NULL" );

    /* Return an unconstrained but valid enum value */
    __CPROVER_assume( eReturn == eReleaseBuffer ||
                      eReturn == eProcessBuffer ||
                      eReturn == eReturnEthernetFrame ||
                      eReturn == eFrameConsumed ||
                      eReturn == eWaitingARPResolution );

    return eReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnMAC */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    NetworkEndPoint_t * pxEndPoint;

    __CPROVER_assert( pxMACAddress != NULL,
                      "FreeRTOS_FindEndPointOnMAC: pxMACAddress != NULL" );

    pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    return pxEndPoint;
}

/* Abstraction of FreeRTOS_FindEndPointOnNetMask */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress,
                                                    uint32_t ulWhere )
{
    NetworkEndPoint_t * pxEndPoint;

    pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    return pxEndPoint;
}

/* Abstraction of pxGetNetworkBufferWithDescriptor */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;

    pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->pucEthernetBuffer = safeMalloc( xRequestedSizeBytes );
        pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
        pxNetworkBuffer->pxInterface = safeMalloc( sizeof( NetworkInterface_t ) );
        pxNetworkBuffer->pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    return pxNetworkBuffer;
}

/* Abstraction of vReturnEthernetFrame */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "vReturnEthernetFrame: pxNetworkBuffer != NULL" );
}

/* The harness test for HandleEthernetPacket */
void HandleEthernetPacket_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xDataLength;

    /* Allocate a network buffer descriptor */
    pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    /* Assume pxNetworkBuffer is non-NULL since HandleEthernetPacket
     * expects a valid descriptor */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Constrain data length to be between minimum Ethernet header size
     * and the maximum MTU size */
    __CPROVER_assume( xDataLength >= sizeof( EthernetHeader_t ) &&
                      xDataLength <= ipconfigNETWORK_MTU );

    /* Allocate the Ethernet buffer within the descriptor */
    pxNetworkBuffer->pucEthernetBuffer = safeMalloc( xDataLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xDataLength;

    /* Allocate and initialise the interface */
    pxNetworkBuffer->pxInterface = safeMalloc( sizeof( NetworkInterface_t ) );

    /* Allocate and initialise the endpoint */
    pxNetworkBuffer->pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    if( pxNetworkBuffer->pxEndPoint != NULL )
    {
        /* Set up endpoint's interface pointer */
        pxNetworkBuffer->pxEndPoint->pxNetworkInterface =
            pxNetworkBuffer->pxInterface;
    }

    /* Call the function under verification */
    HandleEthernetPacket( pxNetworkBuffer );
}