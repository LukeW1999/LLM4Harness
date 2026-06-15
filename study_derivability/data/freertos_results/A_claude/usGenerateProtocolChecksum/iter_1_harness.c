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
#include <stdbool.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"

/* CBMC model includes */
#include "cbmc.h"

/* Maximum packet size for bounded verification */
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE    ( ipTOTAL_ETHERNET_FRAME_SIZE )
#endif

/* Minimum size to hold an IP header */
#define MIN_PACKET_SIZE    ( sizeof( IPPacket_t ) )

void usGenerateProtocolChecksum_harness( void )
{
    /* Nondeterministic inputs */
    uint8_t * pucEthernetBuffer;
    size_t uxBufferLength;
    BaseType_t xOutgoingPacket;
    uint16_t usReturn;

    /* Allocate a nondeterministic buffer length */
    uxBufferLength = nondet_size_t();

    /* Constrain buffer length to a reasonable range:
     * - Must be at least large enough for an Ethernet + IP header
     * - Must not exceed the maximum Ethernet frame size to keep proof tractable */
    __CPROVER_assume( uxBufferLength >= MIN_PACKET_SIZE );
    __CPROVER_assume( uxBufferLength <= BUFFER_SIZE );

    /* Allocate the Ethernet buffer */
    pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* xOutgoingPacket is a boolean-like flag */
    xOutgoingPacket = nondet_BaseType_t();
    __CPROVER_assume( ( xOutgoingPacket == pdTRUE ) || ( xOutgoingPacket == pdFALSE ) );

    /* Call the function under test */
    usReturn = usGenerateProtocolChecksum( pucEthernetBuffer,
                                           uxBufferLength,
                                           xOutgoingPacket );

    /* Postcondition assertions:
     * The function should return one of the defined protocol checksum result codes:
     *   ipCORRECT_CRC     - checksum is correct
     *   ipWRONG_CRC       - checksum is wrong
     *   ipUNHANDLED_PROTOCOL - protocol not handled
     *   ipINVALID_LENGTH  - packet length invalid
     *   0x0000            - checksum has been set (outgoing, correct)
     *   0xFFFF            - valid checksum value (special case for some protocols)
     *
     * The return value must be one of the known valid return codes.
     */
    assert( ( usReturn == ipCORRECT_CRC ) ||
            ( usReturn == ipWRONG_CRC ) ||
            ( usReturn == ipUNHANDLED_PROTOCOL ) ||
            ( usReturn == ipINVALID_LENGTH ) );

    /* Additional postcondition: if computing checksum for outgoing packet,
     * verify the return is a recognized value */
    if( xOutgoingPacket == pdTRUE )
    {
        /* For outgoing packets, the checksum is written into the packet,
         * the function returns ipCORRECT_CRC on success or an error code */
        assert( ( usReturn == ipCORRECT_CRC ) ||
                ( usReturn == ipUNHANDLED_PROTOCOL ) ||
                ( usReturn == ipINVALID_LENGTH ) );
    }
    else
    {
        /* For incoming packets, function validates the existing checksum */
        assert( ( usReturn == ipCORRECT_CRC ) ||
                ( usReturn == ipWRONG_CRC ) ||
                ( usReturn == ipUNHANDLED_PROTOCOL ) ||
                ( usReturn == ipINVALID_LENGTH ) );
    }

    /* Free allocated memory */
    free( pucEthernetBuffer );
}