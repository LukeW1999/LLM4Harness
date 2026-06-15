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
#include "FreeRTOS_UDP_IP.h"

/* CBMC includes. */
#include "cbmc.h"

/*
 * pxUDPPayloadBuffer_to_NetworkBuffer works by subtracting a fixed offset
 * from the payload pointer to recover the NetworkBufferDescriptor_t pointer
 * stored just before the Ethernet buffer.
 *
 * The layout in memory is:
 *   [ NetworkBufferDescriptor_t* ] [ Ethernet header ] [ IP header ] [ UDP header ] [ payload ]
 *
 * The offset from the start of pucEthernetBuffer to the UDP payload is:
 *   ipUDP_PAYLOAD_OFFSET_IPv4  (for IPv4)
 * or
 *   ipUDP_PAYLOAD_OFFSET_IPv6  (for IPv6)
 *
 * Additionally, a pointer to the descriptor is stored at
 *   pucEthernetBuffer - ipBUFFER_PADDING
 */

void pxUDPPayloadBuffer_to_NetworkBuffer_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxIPHeaderSize;
    uint8_t * pucEthernetBuffer;
    void * pvPayloadBuffer;
    size_t uxEthernetBufferSize;

    /* The IP header size is either IPv4 or IPv6. */
    __CPROVER_assume( ( uxIPHeaderSize == ipSIZE_OF_IPv4_HEADER ) ||
                      ( uxIPHeaderSize == ipSIZE_OF_IPv6_HEADER ) );

    /* Calculate the offset from the start of the Ethernet buffer to the UDP payload. */
    size_t uxPayloadOffset = ipSIZE_OF_ETH_HEADER + uxIPHeaderSize + ipSIZE_OF_UDP_HEADER;

    /* The Ethernet buffer needs to be large enough to hold the headers plus some payload. */
    size_t uxMinBufferSize = uxPayloadOffset + 1;
    __CPROVER_assume( uxEthernetBufferSize >= uxMinBufferSize );
    __CPROVER_assume( uxEthernetBufferSize <= ipconfigNETWORK_MTU );

    /* Allocate the network buffer descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /*
     * Allocate the Ethernet buffer with extra space at the front for
     * ipBUFFER_PADDING bytes, which includes the back-pointer to the descriptor.
     */
    pucEthernetBuffer = ( uint8_t * ) safeMalloc( uxEthernetBufferSize + ipBUFFER_PADDING );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Move pointer past the padding area. */
    pucEthernetBuffer += ipBUFFER_PADDING;

    /* Store the back-pointer to the network buffer descriptor just before pucEthernetBuffer. */
    *( ( NetworkBufferDescriptor_t ** ) ( pucEthernetBuffer - ipBUFFER_PADDING ) ) = pxNetworkBuffer;

    /* Set up the network buffer descriptor. */
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = uxEthernetBufferSize;

    /* Compute the payload pointer as the function caller would. */
    pvPayloadBuffer = ( void * ) ( pucEthernetBuffer + uxPayloadOffset );

    /* Call the function under test. */
    ( void ) pxUDPPayloadBuffer_to_NetworkBuffer( pvPayloadBuffer );
}