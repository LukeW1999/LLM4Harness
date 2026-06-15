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
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_Stream_Buffer.h"

/* CBMC includes. */
#include "cbmc.h"

/* Maximum size for stream buffer to keep proofs tractable. */
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE    ipconfigTCP_MSS
#endif

/* Stub for xSendEventStructToIPTask if needed. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    return nondet_BaseType();
}

/* Stub for pxGetNetworkBufferWithDescriptor. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxBuffer = NULL;

    if( nondet_bool() )
    {
        pxBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

        if( pxBuffer != NULL )
        {
            size_t bufSize = xRequestedSizeBytes > 0 ? xRequestedSizeBytes : 1;
            pxBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( bufSize );

            if( pxBuffer->pucEthernetBuffer == NULL )
            {
                free( pxBuffer );
                pxBuffer = NULL;
            }
            else
            {
                pxBuffer->xDataLength = bufSize;
            }
        }
    }

    return pxBuffer;
}

/* Stub for vReleaseNetworkBufferAndDescriptor. */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    if( pxNetworkBuffer != NULL )
    {
        if( pxNetworkBuffer->pucEthernetBuffer != NULL )
        {
            free( pxNetworkBuffer->pucEthernetBuffer );
        }

        free( pxNetworkBuffer );
    }
}

/* Helper to allocate and initialize a stream buffer. */
static StreamBuffer_t * allocate_StreamBuffer( void )
{
    StreamBuffer_t * pxBuffer = NULL;
    size_t bufferSize;

    /* Use a bounded size for the stream buffer. */
    __CPROVER_assume( bufferSize <= BUFFER_SIZE );
    __CPROVER_assume( bufferSize >= sizeof( StreamBuffer_t ) );

    pxBuffer = ( StreamBuffer_t * ) malloc( bufferSize );

    if( pxBuffer != NULL )
    {
        pxBuffer->LENGTH = bufferSize - sizeof( StreamBuffer_t ) + sizeof( pxBuffer->ucArray );
        __CPROVER_assume( pxBuffer->LENGTH >= 1 );

        /* uxStreamBufferGetSpace returns LENGTH - 1 - (uxHead - uxTail) when
         * head >= tail, so constrain indices. */
        __CPROVER_assume( pxBuffer->uxHead < pxBuffer->LENGTH );
        __CPROVER_assume( pxBuffer->uxTail < pxBuffer->LENGTH );
        __CPROVER_assume( pxBuffer->uxMid < pxBuffer->LENGTH );
        __CPROVER_assume( pxBuffer->uxFront < pxBuffer->LENGTH );
    }

    return pxBuffer;
}

/* Helper to allocate and fully initialize a FreeRTOS_Socket_t for TCP. */
static FreeRTOS_Socket_t * allocate_Socket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket != NULL )
    {
        /* Set IPPROTO_TCP so TCP fields are valid. */
        pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;

        /* Nondeterministic TCP state - constrain to valid TCP states. */
        uint8_t tcpState = nondet_uint8_t();
        __CPROVER_assume( tcpState <= eCLOSE_WAIT );
        pxSocket->u.xTCP.ucTCPState = tcpState;

        /* Nondeterministic flags and settings. */
        pxSocket->u.xTCP.usMSS = nondet_uint16_t();
        __CPROVER_assume( pxSocket->u.xTCP.usMSS >= 1 );
        __CPROVER_assume( pxSocket->u.xTCP.usMSS <= ipconfigTCP_MSS );

        pxSocket->u.xTCP.usTimeout = nondet_uint16_t();
        pxSocket->u.xTCP.ucRepCount = nondet_uint8_t();

        pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.tx.ulCurrentSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.tx.ulFINSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.tx.ulFirstSequenceNumber = nondet_uint32_t();
        pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber = nondet_uint32_t();

        /* Allocate TX stream buffer - may be NULL. */
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.txStream = allocate_StreamBuffer();
        }
        else
        {
            pxSocket->u.xTCP.txStream = NULL;
        }

        /* Allocate RX stream buffer - may be NULL. */
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.rxStream = allocate_StreamBuffer();
        }
        else
        {
            pxSocket->u.