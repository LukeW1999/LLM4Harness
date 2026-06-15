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
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/* The maximum size of a network buffer. We need to bound this for CBMC. */
#ifndef NETWORK_BUFFER_SIZE
    #define NETWORK_BUFFER_SIZE    1500U
#endif

/* Proof assumption: pxGetNetworkBufferWithDescriptor may return NULL or a valid
 * buffer. We stub it here to model both cases nondeterministically. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxBuffer = NULL;

    /* Nondeterministically decide whether to return a valid buffer or NULL. */
    if( nondet_bool() )
    {
        pxBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

        if( pxBuffer != NULL )
        {
            /* Allocate a buffer of the requested size. */
            if( xRequestedSizeBytes > 0 )
            {
                __CPROVER_assume( xRequestedSizeBytes <= NETWORK_BUFFER_SIZE );
                pxBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xRequestedSizeBytes );
                pxBuffer->xDataLength = xRequestedSizeBytes;
            }
            else
            {
                pxBuffer->pucEthernetBuffer = NULL;
                pxBuffer->xDataLength = 0;
            }

            pxBuffer->pxInterface = NULL;
            pxBuffer->pxEndPoint = NULL;
        }
    }

    return pxBuffer;
}

/* Stub for vReleaseNetworkBufferAndDescriptor - not needed in this harness
 * but may be referenced by the implementation. */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    /* Nothing to do in the stub. */
}

void pxDuplicateNetworkBufferWithDescriptor_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    NetworkBufferDescriptor_t * pxDuplicateNetworkBuffer;
    size_t uxNewLength;

    /* Allocate a source network buffer descriptor nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* The function may be called with NULL, model that nondeterministically. */
    if( nondet_bool() )
    {
        pxNetworkBuffer = NULL;
    }
    else
    {
        /* Assume pxNetworkBuffer is non-NULL in this branch. */
        __CPROVER_assume( pxNetworkBuffer != NULL );

        /* Set up the ethernet buffer with a bounded nondeterministic size. */
        size_t xDataLength;
        __CPROVER_assume( xDataLength <= NETWORK_BUFFER_SIZE );
        __CPROVER_assume( xDataLength > 0 );

        pxNetworkBuffer->xDataLength = xDataLength;

        /* Allocate the ethernet buffer. */
        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xDataLength );

        /* pucEthernetBuffer could be NULL if malloc fails; allow that. */

        /* Set other fields nondeterministically. */
        pxNetworkBuffer->pxInterface = NULL;
        pxNetworkBuffer->pxEndPoint = NULL;
    }

    /* uxNewLength is the requested size for the duplicate buffer. */
    __CPROVER_assume( uxNewLength <= NETWORK_BUFFER_SIZE );

    /* Call the function under test. */
    pxDuplicateNetworkBuffer = pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, uxNewLength );

    /* Postconditions:
     * 1. If pxNetworkBuffer is NULL, the result should be NULL.
     * 2. If pxDuplicateNetworkBuffer is non-NULL, its xDataLength should be
     *    equal to uxNewLength.
     * 3. If pxDuplicateNetworkBuffer is non-NULL, its pucEthernetBuffer should
     *    be non-NULL (it was allocated).
     */
    if( pxNetworkBuffer == NULL )
    {
        /* When source is NULL, result must be NULL. */
        assert( pxDuplicateNetworkBuffer == NULL );
    }

    if( pxDuplicateNetworkBuffer != NULL )
    {
        /* The data length of the duplicate should match requested size. */
        assert( pxDuplicateNetworkBuffer->xDataLength == uxNewLength );

        /* The ethernet buffer of the duplicate must be valid. */
        assert( pxDuplicateNetworkBuffer->pucEthernetBuffer != NULL );
    }
}