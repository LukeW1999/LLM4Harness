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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_Transmission.h"

/* CBMC proof helpers */
#include "cbmc.h"

/* The function under test */
void TCPReturnPacket_IPv6( FreeRTOS_Socket_t * pxSocket,
                           NetworkBufferDescriptor_t * pxDescriptor,
                           uint32_t ulLen,
                           BaseType_t xReleaseAfterSend );

/* Stub for network interface output */
BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend )
{
    return pdTRUE;
}

void TCPReturnPacket_IPv6_harness( void )
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxDescriptor;
    uint32_t ulLen;
    BaseType_t xReleaseAfterSend;

    /* Allocate socket (may be NULL) */
    if( nondet_bool() )
    {
        pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );
        __CPROVER_assume( pxSocket != NULL );

        /* Initialize socket fields */
        pxSocket->u.xTCP.eTCPState = ( eIPTCPState_t ) nondet_uint();
        pxSocket->u.xTCP.txStream = NULL;
        pxSocket->u.xTCP.rxStream = NULL;
        pxSocket->u.xTCP.pxHandleConnected = NULL;
        pxSocket->u.xTCP.pxHandleReceive = NULL;
        pxSocket->u.xTCP.pxHandleSent = NULL;
        pxSocket->u.xTCP.ucTCPState = nondet_uint8_t();
        pxSocket->u.xTCP.bits.bReuseSocket = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bPassQueued = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bPassAccept = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bWinChange = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bSendKeepAlive = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bTimeOut = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bFinSent = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bFinRecv = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bFinAccepted = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bFinLast = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bRxStopped = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bMallocError = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bCloseAfterSend = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bUserShutdown = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bConnPrepared = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bConnected = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bAcceptConnect = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bDeleteSocket = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bWaitKeepAlive = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bHighWater = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bLostData = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bSendFin = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bFramePortSet = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.bits.bIPv6 = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
        pxSocket->u.xTCP.usMSS = nondet_uint16_t();
        pxSocket->u.xTCP.usTimeout = nondet