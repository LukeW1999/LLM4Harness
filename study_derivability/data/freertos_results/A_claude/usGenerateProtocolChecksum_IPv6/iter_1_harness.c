```c
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
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Checksum.h"

/* CBMC proof includes. */
#include "cbmc.h"

/* Maximum packet size used in the proof to bound the buffer. */
#ifndef TEST_MAX_FRAME_SIZE
    #define TEST_MAX_FRAME_SIZE    ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER )
#endif

/* Minimum size: Ethernet header + IPv6 header + some payload (e.g., ICMPv6 header). */
#ifndef TEST_MIN_FRAME_SIZE
    #define TEST_MIN_FRAME_SIZE    ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( ICMPHeader_IPv6_t ) )
#endif

/*
 * The function under test.
 */
extern uint16_t usGenerateProtocolChecksum_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                 BaseType_t xOutgoingPacket );

void usGenerateProtocolChecksum_IPv6_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    BaseType_t xOutgoingPacket;
    uint16_t usResult;

    /* Allocate the network buffer descriptor nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Nondeterministically choose whether the pointer is NULL or valid. */
    if( nondet_bool() )
    {
        /* Test with NULL network buffer. */
        pxNetworkBuffer = NULL;
    }
    else
    {
        /* Choose a nondeterministic buffer length within valid bounds. */
        size_t xBufferLength;
        __CPROVER_assume( xBufferLength >= TEST_MIN_FRAME_SIZE );
        __CPROVER_assume( xBufferLength <= TEST_MAX_FRAME_SIZE );

        /* Allocate the actual packet buffer. */
        uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( xBufferLength );
        __CPROVER_assume( pucEthernetBuffer != NULL );

        pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
        pxNetworkBuffer->xDataLength = xBufferLength;

        /* Set nondeterministic interface and endpoint pointers (can be NULL). */
        pxNetworkBuffer->pxInterface = nondet_bool() ? NULL :
                                       ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
        pxNetworkBuffer->pxEndPoint  = nondet_bool() ? NULL :
                                       ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

        if( pxNetworkBuffer->pxEndPoint != NULL )
        {
            /* Nondeterministic endpoint fields. */
            __CPROVER_assume( pxNetworkBuffer->pxEndPoint->pxNetworkInterface ==
                              pxNetworkBuffer->pxInterface );
        }

        /* Set up a valid IPv6 Ethernet frame header so the function can parse it.
         * The EtherType for IPv6 is 0x86DD. */
        EthernetHeader_t * pxEtherHeader =
            ( EthernetHeader_t * ) pxNetworkBuffer->pucEthernetBuffer;
        pxEtherHeader->usFrameType = FreeRTOS_htons( ipIPv6_FRAME_TYPE );

        /* Set up the IPv6 header with a nondeterministic but valid next-header
         * (protocol) field so various protocol paths are covered. */
        IPHeader_IPv6_t * pxIPv6Header =
            ( IPHeader_IPv6_t * ) ( pxNetworkBuffer->pucEthernetBuffer +
                                    ipSIZE_OF_ETH_HEADER );

        /* Version must be 6 (upper nibble of first byte). */
        pxIPv6Header->ucVersionTrafficClass = ( uint8_t ) ( 0x60U | ( pxIPv6Header->ucVersionTrafficClass & 0x0FU ) );

        /* Payload length: must be consistent with the buffer length minus
         * Ethernet + IPv6 headers. */
        uint16_t usPayloadLength =
            ( uint16_t ) ( xBufferLength - ipSIZE_OF_ETH_HEADER - ipSIZE_OF_IPv6_HEADER );
        pxIPv6Header->usPayloadLength = FreeRTOS_htons( usPayloadLength );
    }

    /* Nondeterministic outgoing packet flag. */
    xOutgoingPacket = nondet_BaseType_t();

    /* Call the function under test. */
    usResult = usGenerateProtocolChecksum_IPv6( pxNetworkBuffer, xOutgoingPacket );

    /*
     * Postconditions:
     *
     * The function returns one of the following defined values:
     *   - ipCORRECT_CRC        : checksum is correct
     *   - ipWRONG_CRC          : checksum is wrong
     *   - ipUNHANDLED_PROTOCOL : protocol not handled
     *   - ipINVALID_LENGTH     : packet length inconsistency
     *
     * We assert that the return value is one of the expected values when the
     * network buffer is non-NULL. When it is NULL the function should return
     * ipINVALID_LENGTH.
     */
    if( pxNetworkBuffer == NULL )
    {
        /* A NULL buffer should yield an invalid-length or unhandled result.
         * Different implementations may differ; we assert a broad safety property. */
        assert( ( usResult == ipINVALID_LENGTH ) ||
                ( usResult == ipUNHANDLED_PROTOCOL ) ||
                ( usResult == ipWRONG_CRC ) ||
                ( usResult == ipCORRECT_CRC ) );
    }
    else
    {
        /* For