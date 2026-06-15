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

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_Stream_Buffer.h"

/* CBMC model includes. */
#include "cbmc.h"

/* The maximum number of bytes that can be sent in a single call. */
#ifndef SEND_DATA_MAX_SIZE
    #define SEND_DATA_MAX_SIZE    ( ipconfigTCP_MSS )
#endif

/* Proof assumption: stream buffer size is bounded. */
#ifndef STREAM_BUFFER_SIZE
    #define STREAM_BUFFER_SIZE    ( 2 * ipconfigTCP_MSS )
#endif

/*
 * The function under test is declared in FreeRTOS_TCP_IP.c.
 * It is a static function, so we need an extern declaration or
 * it must be exposed via a test interface.
 */
extern BaseType_t SendData( FreeRTOS_Socket_t * pxSocket,
                            const void * pvBuffer,
                            size_t uxDataLength,
                            BaseType_t xFlags );

/*
 * Helper to allocate and initialize a FreeRTOS_Socket_t with
 * nondeterministic but constrained fields for TCP sending.
 */
static FreeRTOS_Socket_t * allocate_socket( void )
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );

    __CPROVER_assume( pxSocket != NULL );

    /* Initialize with nondeterministic values. */
    pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;
    pxSocket->u.xTCP.eTCPState = ( eIPTCPState_t ) nondet_uint8_t();

    /* Constrain TCP state to valid values. */
    __CPROVER_assume( pxSocket->u.xTCP.eTCPState >= eCLOSED );
    __CPROVER_assume( pxSocket->u.xTCP.eTCPState <= eCLOSING );

    /* Nondeterministic bits and timeout fields. */
    pxSocket->u.xTCP.bits.bMallocError = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bCloseAfterSend = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bUserShutdown = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bCloseRequested = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bFinSent = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bWinChange = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bSendKeepAlive = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bTimeoutZero = nondet_BaseType_t();
    pxSocket->u.xTCP.bits.bRxStopped = nondet_BaseType_t();

    pxSocket->xSendBlockTime = nondet_TickType_t();
    pxSocket->u.xTCP.usMSS = nondet_uint16_t();
    __CPROVER_assume( pxSocket->u.xTCP.usMSS > 0 );
    __CPROVER_assume( pxSocket->u.xTCP.usMSS <= ipconfigTCP_MSS );

    /* Allocate the TX stream buffer. */
    StreamBuffer_t * pxTxStream = ( StreamBuffer_t * ) malloc(
        sizeof( StreamBuffer_t ) + STREAM_BUFFER_SIZE );

    if( pxTxStream != NULL )
    {
        pxTxStream->LENGTH = STREAM_BUFFER_SIZE;
        pxTxStream->uxHead = nondet_size_t();
        __CPROVER_assume( pxTxStream->uxHead < STREAM_BUFFER_SIZE );
        pxTxStream->uxTail = nondet_size_t();
        __CPROVER_assume( pxTxStream->uxTail < STREAM_BUFFER_SIZE );
        pxTxStream->uxMidAge = nondet_size_t();
        pxTxStream->uxFront = nondet_size_t();
        __CPROVER_assume( pxTxStream->uxFront < STREAM_BUFFER_SIZE );
    }

    pxSocket->u.xTCP.txStream = pxTxStream;

    /* Allocate the RX stream buffer (may be NULL). */
    StreamBuffer_t * pxRxStream = NULL;

    if( nondet_bool() )
    {
        pxRxStream = ( StreamBuffer_t * ) malloc(
            sizeof( StreamBuffer_t ) + STREAM_BUFFER_SIZE );

        if( pxRxStream != NULL )
        {
            pxRxStream->LENGTH = STREAM_BUFFER_SIZE;
            pxRxStream->uxHead = nondet_size_t();
            __CPROVER_assume( pxRxStream->uxHead < STREAM_BUFFER_SIZE );
            pxRxStream->uxTail = nondet_size_t();
            __CPROVER_assume( pxRxStream->uxTail < STREAM_BUFFER_SIZE );
            pxRxStream->uxMidAge = nondet_size_t();
            pxRxStream->uxFront = nondet_size_t();
            __CPROVER_assume( pxRxStream->uxFront < STREAM_BUFFER_SIZE );
        }
    }

    pxSocket->u.xTCP.rxStream = pxRxStream;

    /* Nondeterministic window size fields. */
    pxSocket->u.xTCP.xTCPWindow.xSize.ulTxWindowLength = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = nondet_uint32_t();
    pxSocket->u