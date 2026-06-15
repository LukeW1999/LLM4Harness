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
#include "FreeRTOS_TCP_IP.h"

/* CBMC includes. */
#include "cbmc.h"

/*
 * The minimum buffer size must be large enough to hold an Ethernet header
 * plus the largest IP header (IPv6 = 40 bytes) plus the largest protocol
 * header we care about (TCP = 20 bytes).
 *
 * ipSIZE_OF_ETH_HEADER        = 14
 * ipSIZE_OF_IPv6_HEADER       = 40
 * ipSIZE_OF_TCP_HEADER        = 20
 * Total minimum                = 74
 *
 * We bound the maximum buffer size to keep CBMC tractable.
 */
#define CBMC_MIN_PACKET_SIZE    ( ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + ipSIZE_OF_TCP_HEADER ) )
#define CBMC_MAX_PACKET_SIZE    ( ( size_t ) ipconfigNETWORK_MTU )

void usGenerateProtocolChecksum_harness()
{
    uint8_t * pucEthernetBuffer;
    size_t uxBufferLength;
    BaseType_t xOutgoingPacket;

    /* Pick a non-deterministic buffer length within tractable bounds. */
    __CPROVER_assume( uxBufferLength >= CBMC_MIN_PACKET_SIZE );
    __CPROVER_assume( uxBufferLength <= CBMC_MAX_PACKET_SIZE );

    /* Allocate a buffer of the chosen length. */
    pucEthernetBuffer = ( uint8_t * ) safeMalloc( uxBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* xOutgoingPacket is a boolean flag – constrain to valid values. */
    __CPROVER_assume( ( xOutgoingPacket == pdTRUE ) || ( xOutgoingPacket == pdFALSE ) );

    /*
     * The function under verification inspects the IP version field inside the
     * buffer to decide whether the packet is IPv4 or IPv6, and then inspects
     * the protocol field to choose TCP/UDP/ICMP handling.  All of those bytes
     * are part of the non-deterministic buffer contents, so every code path
     * inside usGenerateProtocolChecksum is reachable.
     */
    ( void ) usGenerateProtocolChecksum( pucEthernetBuffer, uxBufferLength, xOutgoingPacket );
}