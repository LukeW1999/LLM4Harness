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
#include "list.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_Stream_Buffer.h"

/* CBMC includes. */
#include "cbmc.h"

/* Declaration of the function under test. */
FreeRTOS_Socket_t * prvHandleListen( FreeRTOS_Socket_t * pxSocket,
                                     NetworkBufferDescriptor_t * pxNetworkBuffer );

/* Helper to allocate and initialize a socket nondeterministically. */
static FreeRTOS_Socket_t * allocate_socket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket != NULL )
    {
        __CPROVER_assume( pxSocket != NULL );

        /* Initialize socket fields to nondeterministic values. */
        pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;
        pxSocket->u.xTCP.eTCPState = eTCP_LISTEN;
        pxSocket->u.xTCP.usBacklog = nondet_uint16_t();
        pxSocket->u.xTCP.usChildCount = nondet_uint16_t();

        /* Ensure child count does not exceed backlog to avoid UB. */
        __CPROVER_assume( pxSocket->u.xTCP.usBacklog <= 10 );
        __CPROVER_assume( pxSocket->u.xTCP.usChildCount <= pxSocket->u.xTCP.usBacklog );

        pxSocket->u.xTCP.bits.bReuseSocket = nondet_bool();
        pxSocket->u.xTCP.bits.bPassAccept  = nondet_bool();
        pxSocket->u.xTCP.bits.bPassQueued  = nondet_bool();

        /* Initialize the socket lists. */
        vListInitialise( &( pxSocket->u.xTCP.xBoundTCPSocketsList ) );
        vListInitialise( &( pxSocket->xBoundSocketListItem ) );

        /* Port numbers. */
        pxSocket->usLocalPort  = nondet_uint16_t();
        pxSocket->u.xTCP.usRemotePort = nondet_uint16_t();

        /* IP addresses. */
        pxSocket->u.xTCP.ulRemoteIP = nondet_uint32_t();

        /* Semaphore handle may be NULL. */
        if( nondet_bool() )
        {
            pxSocket->pxUserSemaphore = NULL;
        }
        else
        {
            /* Use a non-NULL placeholder. */
            pxSocket->pxUserSemaphore = ( SemaphoreHandle_t ) malloc( sizeof( StaticSemaphore_t ) );
        }

        /* Event group. */
        if( nondet_bool() )
        {
            pxSocket->xEventGroup = NULL;
        }
        else
        {
            pxSocket->xEventGroup = xEventGroupCreate();
        }

        /* Socket set. */
        pxSocket->pxSocketSet = NULL;

        /* Receive/send callbacks. */
        pxSocket->u.xTCP.pxHandleConnected = NULL;
        pxSocket->u.xTCP.pxHandleReceive   = NULL;
        pxSocket->u.xTCP.pxHandleSent      = NULL;

        /* RX/TX stream buffers - leave as NULL for simplicity. */
        pxSocket->u.xTCP.rxStream = NULL;
        pxSocket->u.xTCP.txStream = NULL;
    }

    return pxSocket;
}

/* Helper to allocate and initialize a network buffer nondeterministically. */
static NetworkBufferDescriptor_t * allocate_network_buffer( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer =
        ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        /* Allocate buffer data of sufficient size for a TCP/IP packet. */
        size_t xBufferSize = sizeof( EthernetHeader_t ) +
                             sizeof( IPHeader_t ) +
                             sizeof( TCPHeader_t ) +
                             40U; /* extra bytes for options */

        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
        __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

        pxNetworkBuffer->xDataLength = xBufferSize;
        __CPROVER_assume( pxNetworkBuffer->xDataLength >= ( sizeof( EthernetHeader_t ) +
                                                             sizeof( IPHeader_t ) +
                                                             sizeof( TCPHeader_t ) ) );

        /* Nondeterministic interface. */
        pxNetworkBuffer->pxInterface = NULL;
        pxNetworkBuffer->pxEndPoint  = NULL;

        /* Fill buffer with nondeterministic data. */
        /* The IP and TCP headers will be interpreted by the function. */
        TCPPacket_t * pxTCPPacket = ( TCPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

        /* Set nondeterministic IP/TCP header fields. */
        pxTCPPacket->xIPHeader.ulSourceIPAddress      = nondet_uint32_t();
        pxTCPPacket->xIPHeader.ulDestinationIPAddress = nondet_uint32_t();
        pxTCPPacket->xIPHeader.ucVersionHeaderLength  = 0x45; /* IPv4, IHL=5 */
        pxTCPPacket->xIPHeader.usLength               = FreeRTOS_htons(
            ( uint16_t )( sizeof( IPHeader_t ) + sizeof( TCPHeader_t ) ) );

        pxTCPPacket->xTCPHeader.usSourcePort      = nondet_uint16_t();
        pxTCPPacket->xTCPHeader.usDestinationPort = nondet_uint16_t();
        pxTC