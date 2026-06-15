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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

#include "cbmc.h"

/* Declaration */
eFrameProcessingResult_t ConsiderFrameForProcessing( NetworkBufferDescriptor_t * const pxBuffer );

/* Stubs */

/* prvProcessEthernetPacket() is proved elsewhere */
void __CPROVER_file_local_FreeRTOS_IP_c_prvProcessEthernetPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer != NULL" );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "pxNetworkBuffer->pucEthernetBuffer != NULL" );
}

/* We assume that the pxGetNetworkBufferWithDescriptor function is implemented correctly and returns a valid data structure. */
/* This is the mock to mimic the correct expected behavior. If this allocation fails, this might invalidate the proof. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    __CPROVER_assume( pxNetworkBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = safeMalloc( xRequestedSizeBytes );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
    return pxNetworkBuffer;
}

/* The harness test proceeds to call ConsiderFrameForProcessing with an unconstrained buffer */
void ConsiderFrameForProcessing_harness()
{
    NetworkBufferDescriptor_t * pxBuffer;
    TickType_t xBlockTimeTicks;
    BaseType_t uBuffSize;

    /* Assume minimum and maximum buffer size expected */
    __CPROVER_assume( uBuffSize > 0 && uBuffSize < ipconfigNETWORK_MTU );

    pxBuffer = pxGetNetworkBufferWithDescriptor( uBuffSize, xBlockTimeTicks );

    ConsiderFrameForProcessing( pxBuffer );
}