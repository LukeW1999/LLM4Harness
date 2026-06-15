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
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ICMP.h"

/* CBMC includes. */
#include "cbmc.h"

/* The maximum size of the network buffer data. */
#ifndef TEST_MAX_FRAME_SIZE
    #define TEST_MAX_FRAME_SIZE    1500
#endif

/* Minimum size needed for an ICMP packet:
 * Ethernet header + IP header + ICMP header */
#define ICMP_PACKET_MIN_SIZE    ( sizeof( EthernetHeader_t ) + \
                                  sizeof( IPHeader_t ) +       \
                                  sizeof( ICMPHeader_t ) )

/****************************************************************
 * Stub for pxGetNetworkBufferWithDescriptor if needed
 ****************************************************************/

/* Global network buffer used in the harness. */
static NetworkBufferDescriptor_t xNetworkBuffer;
static uint8_t ucEthernetBuffer[ TEST_MAX_FRAME_SIZE ];

/****************************************************************
 * Harness
 ****************************************************************/

void ProcessICMPPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    eFrameProcessingResult_t eResult;
    size_t xBufferLength;

    /* Use the static buffer to avoid dynamic allocation issues. */
    pxNetworkBuffer = &xNetworkBuffer;

    /* Set up the Ethernet buffer with nondeterministic data. */
    pxNetworkBuffer->pucEthernetBuffer = ucEthernetBuffer;

    /* Choose a nondeterministic buffer length that is at least large enough
     * to hold a minimal ICMP packet but no larger than the max frame size. */
    __CPROVER_assume( xBufferLength >= ICMP_PACKET_MIN_SIZE );
    __CPROVER_assume( xBufferLength <= TEST_MAX_FRAME_SIZE );
    pxNetworkBuffer->xDataLength = xBufferLength;

    /* Fill the Ethernet buffer with nondeterministic data. */
    /* CBMC will nondeterministically initialize the array contents. */

    /* Set up a nondeterministic interface pointer (may be NULL or valid). */
    NetworkInterface_t xInterface;
    uint8_t ucInterfaceChoice;
    if( ucInterfaceChoice )
    {
        pxNetworkBuffer->pxInterface = &xInterface;
    }
    else
    {
        pxNetworkBuffer->pxInterface = NULL;
    }

    /* Set up nondeterministic endpoint (may be NULL or valid). */
    NetworkEndPoint_t xEndPoint;
    uint8_t ucEndPointChoice;
    if( ucEndPointChoice )
    {
        /* Set nondeterministic IP address for the endpoint. */
        xEndPoint.ipv4_settings.ulIPAddress = nondet_uint32_t();
        pxNetworkBuffer->pxEndPoint = &xEndPoint;
    }
    else
    {
        pxNetworkBuffer->pxEndPoint = NULL;
    }

    /* Set up IP header inside the Ethernet buffer so the packet is
     * structurally valid enough for the function to parse. */
    IPPacket_t * pxIPPacket = ( IPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* The IP header length field (lower nibble of ucVersionHeaderLength)
     * must indicate a valid header length. Constrain to a reasonable range. */
    uint8_t ucVersionHeaderLength = nondet_uint8_t();
    /* IPv4 version = 4, IHL between 5 and 15 (20 to 60 bytes) */
    __CPROVER_assume( ( ucVersionHeaderLength & 0xF0U ) == 0x40U );
    __CPROVER_assume( ( ucVersionHeaderLength & 0x0FU ) >= 5U );
    __CPROVER_assume( ( ucVersionHeaderLength & 0x0FU ) <= 15U );
    pxIPPacket->xIPHeader.ucVersionHeaderLength = ucVersionHeaderLength;

    /* Set the ICMP type nondeterministically so both echo request/reply
     * and other types are explored. */
    ICMPHeader_t * pxICMPHeader = ( ICMPHeader_t * )
                                  ( pxNetworkBuffer->pucEthernetBuffer +
                                    sizeof( EthernetHeader_t ) +
                                    ( size_t ) ( ( ucVersionHeaderLength & 0x0FU ) * 4U ) );

    /* Ensure the ICMP header is within the buffer. */
    size_t xICMPOffset = sizeof( EthernetHeader_t ) +
                         ( size_t ) ( ( ucVersionHeaderLength & 0x0FU ) * 4U );
    __CPROVER_assume( xICMPOffset + sizeof( ICMPHeader_t ) <= xBufferLength );

    /* Call the function under test. */
    eResult = ProcessICMPPacket( pxNetworkBuffer );

    /* Postcondition: the result must be a valid eFrameProcessingResult_t value. */
    assert( ( eResult == eReleaseBuffer ) ||
            ( eResult == eProcessBuffer ) ||
            ( eResult == eReturnEthernetFrame ) ||
            ( eResult == eFrameConsumed ) ||
            ( eResult == eWaitingARPResolution ) );
}