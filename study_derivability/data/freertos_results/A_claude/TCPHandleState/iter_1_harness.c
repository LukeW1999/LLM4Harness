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

#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_TCP_State_Handling.h"
#include "FreeRTOS_Stream_Buffer.h"

/* CBMC proof includes. */
#include "cbmc.h"

/* The maximum segment size used in this proof. */
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE    ( ipconfigTCP_MSS )
#endif

/* Proof assumptions for stream buffer size. */
#ifndef STREAM_BUFFER_SIZE
    #define STREAM_BUFFER_SIZE    ( 1500 )
#endif

/* Helper to allocate a NetworkBufferDescriptor_t with a valid EthernetBuffer. */
static NetworkBufferDescriptor_t * allocate_NetworkBufferDescriptor( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->xDataLength = sizeof( TCPPacket_t );
        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( sizeof( TCPPacket_t ) + ipBUFFER_PADDING );

        if( pxNetworkBuffer->pucEthernetBuffer != NULL )
        {
            pxNetworkBuffer->pucEthernetBuffer += ipBUFFER_PADDING;
        }
        else
        {
            free( pxNetworkBuffer );
            pxNetworkBuffer = NULL;
        }
    }

    return pxNetworkBuffer;
}

/* Helper to allocate a StreamBuffer_t. */
static StreamBuffer_t * allocate_StreamBuffer( void )
{
    StreamBuffer_t * pxBuffer = ( StreamBuffer_t * ) malloc( sizeof( StreamBuffer_t ) + STREAM_BUFFER_SIZE );

    if( pxBuffer != NULL )
    {
        pxBuffer->LENGTH = STREAM_BUFFER_SIZE;
        pxBuffer->ucArray[ 0 ] = 0U;
    }

    return pxBuffer;
}

/* Helper to allocate and initialize a FreeRTOS_Socket_t for TCP. */
static FreeRTOS_Socket_t * allocate_TCPSocket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket == NULL )
    {
        return NULL;
    }

    /* Initialize the socket to zero. */
    ( void ) memset( pxSocket, 0, sizeof( FreeRTOS_Socket_t ) );

    /* Set IPPROTO to TCP. */
    pxSocket->ucProtocol = ( uint8_t ) FREERTOS_IPPROTO_TCP;

    /* Set a valid TCP state - must be one of the valid eTCP_STATE values. */
    uint8_t ucState = nondet_uint8();
    __CPROVER_assume( ucState <= ( uint8_t ) eCLOSE_WAIT );
    pxSocket->u.xTCP.eTCPState = ( eIPTCPState_t ) ucState;

    /* Set nondeterministic fields for TCP. */
    pxSocket->u.xTCP.usTimeout = nondet_uint16();
    pxSocket->u.xTCP.ucTCPState = nondet_uint8();
    pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.xTCPWindow.tx.ulCurrentSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.xTCPWindow.tx.ulFINSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.xTCPWindow.rx.ulFINSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber = nondet_uint32();
    pxSocket->u.xTCP.bits.bFinSent = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bFinAccepted = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bFinRecv = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bFinAcked = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bReuseSocket = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bPassQueued = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bPassAccept = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bWinChange = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bSendKeepAlive = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bWaitKeepAlive = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.bits.bConnPrepared = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;

    /* Optionally allocate stream buffers. */
    if( nondet_bool() )
    {
        pxSocket->u.xTCP.txStream = allocate_StreamBuffer();
    }
    else
    {
        pxSocket->u.xTCP.txStream = NULL