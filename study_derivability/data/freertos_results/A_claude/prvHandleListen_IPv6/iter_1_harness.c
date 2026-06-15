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

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_Transmission.h"
#include "FreeRTOS_TCP_State_Handling.h"
#include "FreeRTOS_TCP_State_Handling_IPv6.h"

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
FreeRTOS_Socket_t * prvHandleListen_IPv6( FreeRTOS_Socket_t * pxSocket,
                                          NetworkBufferDescriptor_t * pxNetworkBuffer );

/* Stub for pvPortMalloc used in socket creation. */
void * pvPortMalloc( size_t xSize )
{
    /* Return nondeterministic allocation (may be NULL to simulate failure). */
    void * pvReturn = malloc( xSize );
    return pvReturn;
}

void vPortFree( void * pv )
{
    free( pv );
}

/* Stub for xTaskGetTickCount */
TickType_t xTaskGetTickCount( void )
{
    return ( TickType_t ) nondet_uint32_t();
}

/* Helper to allocate and initialize a nondeterministic FreeRTOS_Socket_t */
static FreeRTOS_Socket_t * allocate_and_init_socket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket != NULL )
    {
        /* Fill with nondeterministic data. */
        __CPROVER_havoc_object( pxSocket );

        /* Set socket family to IPv6. */
        pxSocket->bits.bIsIPv6 = pdTRUE_UNSIGNED;

        /* Set socket type to TCP. */
        pxSocket->ucProtocol = ( uint8_t ) FREERTOS_IPPROTO_TCP;

        /* Ensure the socket is in the listen state. */
        pxSocket->u.xTCP.eTCPState = eTCP_LISTEN;

        /* Bound port must be valid. */
        uint16_t usPort = nondet_uint16_t();
        __CPROVER_assume( usPort != 0U );
        pxSocket->usLocalPort = usPort;

        /* Set up nondeterministic backlog values within reasonable bounds. */
        BaseType_t xBacklog = nondet_BaseType_t();
        __CPROVER_assume( xBacklog >= 0 );
        __CPROVER_assume( xBacklog <= ( BaseType_t ) 10 );
        pxSocket->u.xTCP.usBacklog = ( uint16_t ) xBacklog;

        BaseType_t xChildCount = nondet_BaseType_t();
        __CPROVER_assume( xChildCount >= 0 );
        __CPROVER_assume( xChildCount <= ( BaseType_t ) 10 );
        pxSocket->u.xTCP.usChildCount = ( uint16_t ) xChildCount;

        /* Initialize the list of child sockets. */
        vListInitialise( &( pxSocket->u.xTCP.xChildListHead ) );

        /* Set up the IPv6 address for the listening socket. */
        memset( pxSocket->xLocalAddress.xIP_IPv6.ucBytes, 0, ipSIZE_OF_IPv6_ADDRESS );
    }

    return pxSocket;
}

/* Helper to allocate and initialize a nondeterministic NetworkBufferDescriptor_t */
static NetworkBufferDescriptor_t * allocate_and_init_network_buffer( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer =
        ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        __CPROVER_havoc_object( pxNetworkBuffer );

        /* Allocate buffer for the network packet data. */
        size_t xDataLength = sizeof( TCPPacket_IPv6_t ) + ipSIZE_OF_ETH_HEADER;
        __CPROVER_assume( xDataLength >= sizeof( TCPPacket_IPv6_t ) );

        uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( xDataLength + ipIP_TYPE_OFFSET );

        if( pucEthernetBuffer != NULL )
        {
            /* Point past the IP type offset. */
            pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer + ipIP_TYPE_OFFSET;
            pxNetworkBuffer->xDataLength = xDataLength;

            /* Havoc the packet data. */
            __CPROVER_havoc_slice( pxNetworkBuffer->pucEthernetBuffer, xDataLength );
        }
        else
        {
            /* If buffer allocation failed, set to a minimal valid state. */
            pxNetworkBuffer->pucEthernetBuffer = NULL;
            pxNetworkBuffer->xDataLength = 0;
        }

        /* Set up the interface pointer. */
        NetworkInterface_t * pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
        if( pxInterface != NULL )
        {
            __CPROVER_havoc_object( pxInterface );
        }
        pxNetworkBuffer->pxInterface = pxInterface;

        /* Set up the endpoint pointer. */
        NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        if( pxEndPoint != NULL )
        {
            __CPROVER_havoc_object( pxEndPoint );
            pxEndPoint->bits.bIPv6 = pdTRUE_UNSIGNED;
        }
        pxNetworkBuffer->pxEndPoint = pxEndPoint;
    }

    return pxNetworkBuffer;
}

void prvHandleListen_IPv6_harness( void )
{
    /* Allocate and initialize the listening socket. */
    FreeRTOS_Socket_t * pxSocket = allocate_and_init_socket();

    /* The socket must not be NULL for the function to operate correctly. */
    __CPROVER_assume( pxSocket != NULL );

    /* Allocate and initialize the network buffer.