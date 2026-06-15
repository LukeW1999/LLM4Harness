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

/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv6.h"
#include "FreeRTOS_IPv6_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/*
 * The minimum buffer size needed to contain an Ethernet header + IPv6 header.
 * ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER = 14 + 40 = 54.
 */
#define MINIMUM_IPV6_PACKET_SIZE    ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER )

/*
 * The maximum packet size we consider in order to keep the proof tractable.
 * We use ipconfigNETWORK_MTU as the upper bound, consistent with the rest
 * of the CBMC harnesses in this module.
 */
#define HARNESS_BUFFER_SIZE_MAX     ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER )

void harness()
{
    uint8_t * pucEthernetBuffer;
    size_t uxBufferLength;
    BaseType_t xOutgoingPacket;
    uint16_t usResult;

    /* Choose a non-deterministic buffer length that is large enough to contain
     * at least the Ethernet + IPv6 headers, and small enough to keep the proof
     * tractable. */
    __CPROVER_assume( uxBufferLength >= MINIMUM_IPV6_PACKET_SIZE );
    __CPROVER_assume( uxBufferLength <= HARNESS_BUFFER_SIZE_MAX );

    /* Allocate and assume a non-NULL Ethernet buffer of the chosen length. */
    pucEthernetBuffer = ( uint8_t * ) safeMalloc( uxBufferLength );
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* xOutgoingPacket can be any BaseType_t value (pdTRUE / pdFALSE). */
    /* No constraint needed; CBMC will consider all possible values. */

    /* Call the function under verification. */
    usResult = usGenerateProtocolChecksum( pucEthernetBuffer,
                                          uxBufferLength,
                                          xOutgoingPacket );

    /* usResult is used to prevent the compiler / CBMC from optimising away
     * the call. */
    ( void ) usResult;
}