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

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Ethernet.h"

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
eFrameProcessingResult_t ConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer );

void ConsiderFrameForProcessing_harness( void )
{
    /* Allocate a buffer large enough to hold an Ethernet frame header. */
    uint8_t * pucEthernetBuffer;
    eFrameProcessingResult_t eResult;

    /* Allocate nondeterministic buffer for the Ethernet frame.
     * The buffer must be at least the size of an Ethernet header. */
    pucEthernetBuffer = ( uint8_t * ) malloc( sizeof( EthernetHeader_t ) );

    /* Assume the pointer is valid (non-null). */
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Fill the buffer with nondeterministic data to simulate any possible
     * Ethernet frame header content. */
    for( size_t i = 0; i < sizeof( EthernetHeader_t ); i++ )
    {
        pucEthernetBuffer[ i ] = nondet_uint8_t();
    }

    /* Call the function under test. */
    eResult = ConsiderFrameForProcessing( pucEthernetBuffer );

    /* Assert postconditions:
     * The result must be one of the valid eFrameProcessingResult_t values.
     * Valid values are eProcessBuffer or eReleaseBuffer. */
    assert( ( eResult == eProcessBuffer ) || ( eResult == eReleaseBuffer ) );

    /* Free the allocated buffer. */
    free( pucEthernetBuffer );
}