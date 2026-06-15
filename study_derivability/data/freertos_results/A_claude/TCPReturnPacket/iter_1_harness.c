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

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_Stream_Buffer.h"
#include "FreeRTOS_TCP_WIN.h"

/* CBMC includes. */
#include "cbmc.h"

/* NetworkBufferDescriptor_t allocation helper */
NetworkBufferDescriptor_t * allocate_NetworkBufferDescriptor( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        /* Allocate enough space for headers and payload */
        pxNetworkBuffer->xDataLength = nondet_uint32_t();
        __CPROVER_assume( pxNetworkBuffer->xDataLength >= ( ipSIZE_OF_ETH_HEADER +
                                                            ipSIZE_OF_IPv4_HEADER +
                                                            ipSIZE_OF_TCP_HEADER ) );
        __CPROVER_assume( pxNetworkBuffer->xDataLength <= ipconfigNETWORK_MTU );

        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) safeMalloc( pxNetworkBuffer->xDataLength + ipIP_TYPE_OFFSET );

        if( pxNetworkBuffer->pucEthernetBuffer != NULL )
        {
            /* Adjust pointer to account for the IP type offset */
            pxNetworkBuffer->pucEthernetBuffer += ipIP_TYPE_OFFSET;
        }
        else
        {
            /* If buffer allocation fails, set data length to 0 */
            pxNetworkBuffer->xDataLength = 0;
        }

        pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );
        pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    return pxNetworkBuffer;
}

/* FreeRTOS_Socket_t allocation helper */
FreeRTOS_Socket_t * allocate_FreeRTOS_Socket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) safeMalloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket != NULL )
    {
        /* Initialize TCP-specific fields */
        pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;

        /* Set up the network buffer for the socket */
        pxSocket->u.xTCP.pxAckMessage = allocate_NetworkBufferDescriptor();

        /* Nondeterministic values for socket state */
        pxSocket->u.xTCP.eTCPState = nondet_uint32_t();
        __CPROVER_assume( pxSocket->u.xTCP.eTCPState <= eCLOSED );

        pxSocket->u.xTCP.usMSS = nondet_uint16_t();
        __CPROVER_assume( pxSocket->u.xTCP.usMSS >= 1 );

        pxSocket->u.xTCP.usInitMSS = nondet_uint16_t();
        __CPROVER_assume( pxSocket->u.xTCP.usInitMSS >= 1 );

        /* Window size fields */
        pxSocket->u.xTCP.ulWindowSize = nondet_uint32_t();
        pxSocket->u.xTCP.ulRxCurWinSize = nondet_uint32_t();

        /* Sequence numbers */
        pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.tx.ulCurrentSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = nondet_uint32_t();

        /* IP and port information */
        pxSocket->u.xTCP.usRemotePort = nondet_uint16_t();
        pxSocket->usLocalPort = nondet_uint16_t();

        /* Peer address */
        pxSocket->u.xTCP.xRemoteIP.ulIP_IPv4 = nondet_uint32_t();

        /* Interface */
        pxSocket->pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    }

    return pxSocket;
}

void TCPReturnPacket_harness( void )
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxDescriptor;
    uint32_t ulLen;
    BaseType_t xReleaseAfterSend;

    /* Allocate socket (can be NULL) */
    if( nondet_bool() )
    {
        pxSocket = allocate_FreeRTOS_Socket();
    }
    else
    {
        pxSocket = NULL;
    }

    /* Allocate network buffer descriptor (can be NULL) */
    if( nondet_bool() )
    {
        pxDescriptor = allocate_NetworkBufferDescriptor();
    }
    else
    {
        pxDescriptor = NULL;
    }

    /* Nondeterministic length */
    ulLen = nondet_uint32_t();
    __CPROVER_assume( ulLen <= ipconfigNETWORK_MTU );

    /* Nondeterministic release flag */
    xReleaseAfterSend = nondet_BaseType_t();

    /* Constrain the network buffer to have valid data if non-NULL */
    if( pxDescriptor != NULL && pxDescriptor->pucEthernetBuffer != NULL )
    {
        __CPROVER_assume( pxDescriptor->xDataLength >= ( ipSIZE_OF_ETH_HEADER +
                                                         ipSIZE_OF_IPv4_HEADER +
                                                         ipSIZE_OF_TCP_HEADER ) );
        __CPROVER_assume( pxDescriptor->xDataLength <= i