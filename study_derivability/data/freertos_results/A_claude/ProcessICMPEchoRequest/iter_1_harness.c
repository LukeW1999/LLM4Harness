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
#include "FreeRTOS_ICMP.h"

/* CBMC includes. */
#include "cbmc.h"

/* The maximum size of the data payload in an ICMP echo request/reply packet. */
#ifndef ipconfigNETWORK_MTU
    #define ipconfigNETWORK_MTU    1500
#endif

/* Maximum Ethernet frame size. */
#define ETHERNET_FRAME_SIZE    ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER )

/* Proof harness for ProcessICMPEchoRequest */
void ProcessICMPEchoRequest_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xBufferLength;
    uint8_t * pucEthernetBuffer;

    /* Allocate a network buffer descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* The buffer length must be at least large enough to hold the minimum
     * required headers: Ethernet + IP + ICMP. */
    xBufferLength = nondet_size_t();
    __CPROVER_assume( xBufferLength >= sizeof( ICMPPacket_t ) );
    __CPROVER_assume( xBufferLength <= ETHERNET_FRAME_SIZE );

    /* Allocate the ethernet buffer. */
    pucEthernetBuffer = ( uint8_t * ) malloc( xBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Initialize network buffer descriptor fields. */
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = xBufferLength;

    /* The interface pointer may or may not be set. */
    pxNetworkBuffer->pxInterface = nondet_bool() ? NULL : ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );

    /* The end point pointer may or may not be set. */
    pxNetworkBuffer->pxEndPoint = nondet_bool() ? NULL : ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    if( pxNetworkBuffer->pxEndPoint != NULL )
    {
        /* Initialize endpoint IP address fields. */
        pxNetworkBuffer->pxEndPoint->ipv4_settings.ulIPAddress = nondet_uint32_t();
    }

    /* The pucEthernetBuffer contains the ICMP packet. Cast it to get the
     * ICMPPacket structure and set nondeterministic values. */
    ICMPPacket_t * pxICMPPacket = ( ICMPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Set nondeterministic IP header fields. */
    pxICMPPacket->xIPHeader.ulDestinationIPAddress = nondet_uint32_t();
    pxICMPPacket->xIPHeader.ulSourceIPAddress = nondet_uint32_t();
    pxICMPPacket->xIPHeader.usLength = nondet_uint16_t();
    pxICMPPacket->xIPHeader.ucVersionHeaderLength = nondet_uint8_t();
    pxICMPPacket->xIPHeader.ucDifferentiatedServicesCode = nondet_uint8_t();
    pxICMPPacket->xIPHeader.usIdentification = nondet_uint16_t();
    pxICMPPacket->xIPHeader.usFragmentOffset = nondet_uint16_t();
    pxICMPPacket->xIPHeader.ucTimeToLive = nondet_uint8_t();
    pxICMPPacket->xIPHeader.ucProtocol = ipPROTOCOL_ICMP;
    pxICMPPacket->xIPHeader.usHeaderChecksum = nondet_uint16_t();

    /* Set nondeterministic ICMP header fields. */
    pxICMPPacket->xICMPHeader.ucTypeOfMessage = ipICMP_ECHO_REQUEST;
    pxICMPPacket->xICMPHeader.ucTypeOfService = nondet_uint8_t();
    pxICMPPacket->xICMPHeader.usChecksum = nondet_uint16_t();
    pxICMPPacket->xICMPHeader.usIdentifier = nondet_uint16_t();
    pxICMPPacket->xICMPHeader.usSequenceNumber = nondet_uint16_t();

    /* Set nondeterministic Ethernet header fields. */
    ( void ) memset( pxICMPPacket->xEthernetHeader.xDestinationAddress.ucBytes,
                     nondet_uint8_t(),
                     sizeof( MACAddress_t ) );
    ( void ) memset( pxICMPPacket->xEthernetHeader.xSourceAddress.ucBytes,
                     nondet_uint8_t(),
                     sizeof( MACAddress_t ) );
    pxICMPPacket->xEthernetHeader.usFrameType = nondet_uint16_t();

    /* Call the function under test. */
    eARPLookupResult_t xResult = ProcessICMPEchoRequest( pxNetworkBuffer );

    /* Assert postconditions:
     * The return value must be one of the valid eARPLookupResult_t values,
     * or an eFrameProcessed value that is valid for ICMP processing. */
    assert( ( xResult == eReturnEthernetFrame ) ||
            ( xResult == eReleaseBuffer ) ||
            ( xResult == eProcessBuffer ) ||
            ( xResult == eWaitingARPResolution ) );
}