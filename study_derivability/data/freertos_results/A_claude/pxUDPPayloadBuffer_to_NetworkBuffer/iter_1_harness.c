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

#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"

/* The function under test. */
NetworkBufferDescriptor_t * pxUDPPayloadBuffer_to_NetworkBuffer( const void * pvBuffer );

/*
 * The harness allocates a NetworkBufferDescriptor_t with its associated
 * buffer that is large enough to hold the Ethernet + IP + UDP headers
 * plus some payload, sets up the internal pointer so that
 * pxUDPPayloadBuffer_to_NetworkBuffer can walk back from the payload
 * pointer to the descriptor, and then verifies the postcondition that
 * the returned descriptor pointer equals the one that was set up.
 */
void pxUDPPayloadBuffer_to_NetworkBuffer_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint8_t *                   pucEthernetBuffer;
    void *                      pvPayloadBuffer;
    NetworkBufferDescriptor_t * pxReturned;

    /*
     * The minimum offset from the start of the Ethernet buffer to the
     * UDP payload is:
     *   sizeof(NetworkBufferDescriptor_t *) -- hidden back-pointer
     *   + ipSIZE_OF_ETH_HEADER
     *   + ipSIZE_OF_IPv4_HEADER   (or IPv6, but we use IPv4 here)
     *   + ipSIZE_OF_UDP_HEADER
     *
     * We allocate a buffer that is exactly this size plus some
     * nondeterministic payload bytes so that CBMC can explore
     * different sizes while still satisfying alignment requirements.
     */
    size_t uxPayloadOffset = sizeof( NetworkBufferDescriptor_t * ) +
                             ipSIZE_OF_ETH_HEADER +
                             ipSIZE_OF_IPv4_HEADER +
                             ipSIZE_OF_UDP_HEADER;

    /* Nondeterministic extra payload length (bounded to keep state space small). */
    size_t uxPayloadLength;
    __CPROVER_assume( uxPayloadLength <= ipconfigNETWORK_MTU );

    size_t uxBufferSize = uxPayloadOffset + uxPayloadLength;
    __CPROVER_assume( uxBufferSize >= uxPayloadOffset ); /* no overflow */

    /* Allocate the raw Ethernet buffer. */
    pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Allocate the descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /*
     * Store the back-pointer to the descriptor at the very beginning of
     * the Ethernet buffer (this is the convention used by the TCP/IP
     * stack and by pxUDPPayloadBuffer_to_NetworkBuffer).
     */
    *( ( NetworkBufferDescriptor_t ** ) pucEthernetBuffer ) = pxNetworkBuffer;

    /*
     * Point the descriptor's pucEthernetBuffer past the hidden
     * back-pointer so that the stack's normal header offsets work.
     */
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer + sizeof( NetworkBufferDescriptor_t * );
    pxNetworkBuffer->xDataLength       = uxBufferSize - sizeof( NetworkBufferDescriptor_t * );

    /*
     * The UDP payload starts after the Ethernet + IP + UDP headers
     * inside the descriptor's visible buffer region.
     */
    pvPayloadBuffer = ( void * )( pxNetworkBuffer->pucEthernetBuffer +
                                  ipSIZE_OF_ETH_HEADER +
                                  ipSIZE_OF_IPv4_HEADER +
                                  ipSIZE_OF_UDP_HEADER );

    /* Call the function under test. */
    pxReturned = pxUDPPayloadBuffer_to_NetworkBuffer( pvPayloadBuffer );

    /*
     * Postcondition: when the input pointer is a valid UDP payload
     * buffer (i.e. it sits inside a properly initialised network
     * buffer), the function must return the corresponding descriptor.
     */
    assert( pxReturned == pxNetworkBuffer );
}