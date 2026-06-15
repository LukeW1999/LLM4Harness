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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_Transmission.h"
#include "FreeRTOS_TCP_State_Handling.h"
#include "FreeRTOS_Stream_Buffer.h"

/* CBMC includes. */
#include "cbmc.h"
#include "../../utility/memory_assignments.c"

/* Abstraction of uxIPHeaderSizeSocket. This test case only uses IPv4 path. */
size_t uxIPHeaderSizeSocket( const FreeRTOS_Socket_t * pxSocket )
{
    return ipSIZE_OF_IPv4_HEADER;
}

/* Abstraction of uxIPHeaderSizePacket. This test case only uses IPv4 path. */
size_t uxIPHeaderSizePacket( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return ipSIZE_OF_IPv4_HEADER;
}

/* Abstraction of prvTCPAddTxData. */
int32_t prvTCPAddTxData( FreeRTOS_Socket_t * pxSocket )
{
    int32_t xReturn;

    __CPROVER_assert( pxSocket != NULL, "pxSocket cannot be NULL" );

    return xReturn;
}

/* Abstraction of prvTCPPrepareSend. */
int32_t prvTCPPrepareSend( FreeRTOS_Socket_t * pxSocket,
                           NetworkBufferDescriptor_t ** ppxNetworkBuffer,
                           UBaseType_t uxOptionsLength )
{
    int32_t xReturn;

    __CPROVER_assert( pxSocket != NULL, "pxSocket cannot be NULL" );
    __CPROVER_assert( ppxNetworkBuffer != NULL, "ppxNetworkBuffer cannot be NULL" );

    return xReturn;
}

/* Abstraction of prvTCPReturnPacket. */
void prvTCPReturnPacket( FreeRTOS_Socket_t * pxSocket,
                         NetworkBufferDescriptor_t * pxDescriptor,
                         uint32_t ulLen,
                         BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxSocket != NULL, "pxSocket cannot be NULL" );
}

/* Abstraction of xTCPWindowTxDone. */
BaseType_t xTCPWindowTxDone( const TCPWindow_t * pxWindow )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

/* Abstraction of ulTCPWindowTxGet. */
uint32_t ulTCPWindowTxGet( TCPWindow_t * pxWindow,
                           uint32_t ulWindowSize,
                           int32_t * plPosition )
{
    uint32_t ulReturn;

    __CPROVER_assert( pxWindow != NULL, "pxWindow cannot be NULL" );
    __CPROVER_assert( plPosition != NULL, "plPosition cannot be NULL" );

    return ulReturn;
}

/* Abstraction of uxStreamBufferGet. */
size_t uxStreamBufferGet( StreamBuffer_t * const pxBuffer,
                          size_t uxOffset,
                          uint8_t * const pucData,
                          size_t uxMaxCount,
                          BaseType_t xPeek )
{
    size_t uxReturn;

    __CPROVER_assert( pxBuffer != NULL, "pxBuffer cannot be NULL" );

    return uxReturn;
}

/* Abstraction of xTCPWindowTxAdd. */
int32_t xTCPWindowTxAdd( TCPWindow_t * pxWindow,
                         uint32_t ulLength,
                         int32_t lPosition,
                         int32_t lMax )
{
    int32_t xReturn;

    __CPROVER_assert( pxWindow != NULL, "pxWindow cannot be NULL" );

    return xReturn;
}

/* Abstraction of vTCPStateChange. */
void vTCPStateChange( FreeRTOS_Socket_t * pxSocket,
                      enum eTCP_STATE eTCPState )
{
    __CPROVER_assert( pxSocket != NULL, "pxSocket cannot be NULL" );
}

/* Abstraction of FreeRTOS_inet_ntop. */
const char * FreeRTOS_inet_ntop( BaseType_t xAddressFamily,
                                 const void * pvSource,
                                 char * pcDestination,
                                 socklen_t uxSize )
{
    return pcDestination;
}

/* Abstraction of xTCPWindowLoggingLevel. */
BaseType_t xTCPWindowLoggingLevel = 0;

void harness()
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xDataLength;

    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* Socket must be non-NULL. */
    __CPROVER_assume( pxSocket != NULL );

    /* The TX stream buffer may or may not be present. */
    if( nondet_bool() )
    {
        pxSocket->u.xTCP.txStream = ( StreamBuffer_t * ) safeMalloc( sizeof( StreamBuffer_t ) );
    }
    else
    {
        pxSocket->u.xTCP.txStream = NULL;
    }

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    /* The length of buffer must be larger than or equal to TCP minimum requirement. */
    __CPROVER_assume( xDataLength >= sizeof( TCPPacket_t ) && xDataLength <= ipconfigNETWORK_MTU );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) safeMalloc( xDataLength );
        __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
        pxNetworkBuffer->xDataLength = xDataLength;
        pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) safeM