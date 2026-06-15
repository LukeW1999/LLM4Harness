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

/* CBMC includes. */
#include "cbmc.h"

/*
 * Abstraction of pxGetNetworkBufferWithDescriptor.
 * This function is proven separately. Here we return either NULL or a
 * freshly allocated descriptor with a fresh ethernet buffer of the
 * requested size so that the copy inside
 * pxDuplicateNetworkBufferWithDescriptor has valid memory to write to.
 */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->pucEthernetBuffer = safeMalloc( xRequestedSizeBytes );

        if( pxNetworkBuffer->pucEthernetBuffer == NULL )
        {
            /* If we cannot allocate the buffer, return NULL to simulate
             * allocation failure. */
            pxNetworkBuffer = NULL;
        }
        else
        {
            pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
        }
    }

    return pxNetworkBuffer;
}

/*
 * Abstraction of vReleaseNetworkBufferAndDescriptor.
 * This function is proven separately.
 */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "pxNetworkBuffer should not be NULL when releasing" );
}

void pxDuplicateNetworkBufferWithDescriptor_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxNewLength;
    size_t uxEthernetBufferLength;

    /* Allocate the source network buffer descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Constrain the ethernet buffer length to avoid very large allocations
     * while still covering meaningful cases. */
    __CPROVER_assume( uxEthernetBufferLength > 0 );
    __CPROVER_assume( uxEthernetBufferLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER + ipconfigBUFFER_PADDING );

    pxNetworkBuffer->pucEthernetBuffer = safeMalloc( uxEthernetBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* xDataLength must not exceed the allocated ethernet buffer length. */
    __CPROVER_assume( pxNetworkBuffer->xDataLength <= uxEthernetBufferLength );
    __CPROVER_assume( pxNetworkBuffer->xDataLength > 0 );

    /* uxNewLength is the requested size for the duplicate's buffer.
     * Constrain it similarly to avoid enormous allocations. */
    __CPROVER_assume( uxNewLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER + ipconfigBUFFER_PADDING );

    /* Call the function under verification. */
    ( void ) pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, uxNewLength );
}