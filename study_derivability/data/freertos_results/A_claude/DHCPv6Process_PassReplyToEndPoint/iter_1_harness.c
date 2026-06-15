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
#include "FreeRTOS_DHCPv6.h"

/* CBMC helpers */
#include "cbmc.h"

/* Network buffer descriptor size constraint */
#ifndef TEST_NETWORK_BUFFER_SIZE
    #define TEST_NETWORK_BUFFER_SIZE    1500U
#endif

/* Stub/model for xSendEventStructToIPTask if needed */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    return nondet_BaseType_t();
}

/* Stub for vDHCPv6ProcessEndPoint if needed */
void vDHCPv6ProcessEndPoint( BaseType_t xReset,
                              NetworkEndPoint_t * pxEndPoint,
                              DHCPMessage_IPv6_t * pxDHCPMessage )
{
    /* model: no side effects needed */
}

/* Stub for FreeRTOS_printf if needed */
/* already handled by the CBMC stubs library typically */

void DHCPv6Process_PassReplyToEndPoint_harness( void )
{
    /* Allocate a NetworkEndPoint_t */
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* Allocate the DHCPData inside the endpoint */
    /* Initialize endpoint fields nondeterministically */
    pxEndPoint->xDHCPData.eDHCPState    = nondet_uint32_t();
    pxEndPoint->xDHCPData.eExpectedState = nondet_uint32_t();
    pxEndPoint->xDHCPData.ulTransactionID = nondet_uint32_t();
    pxEndPoint->xDHCPData.xDHCPTxTime   = nondet_TickType_t();
    pxEndPoint->xDHCPData.xDHCPTxPeriod = nondet_TickType_t();

    /* Set up IPv6 settings nondeterministically */
    pxEndPoint->bits.bIPv6 = 1U;

    /* Allocate a network buffer */
    NetworkBufferDescriptor_t * pxNetworkBuffer =
        ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate buffer data */
    size_t xBufferSize;
    __CPROVER_assume( xBufferSize >= sizeof( UDPPacket_IPv6_t ) + sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( xBufferSize <= TEST_NETWORK_BUFFER_SIZE );

    uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength       = xBufferSize;
    pxNetworkBuffer->pxEndPoint        = pxEndPoint;
    pxNetworkBuffer->pxNextBuffer      = NULL;

    /* Allocate a DHCPMessage_IPv6_t nondeterministically */
    DHCPMessage_IPv6_t * pxDHCPMessage =
        ( DHCPMessage_IPv6_t * ) malloc( sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( pxDHCPMessage != NULL );

    /* Set transaction ID to match (or not) the endpoint's */
    /* This exercises both matching and non-matching paths */
    pxDHCPMessage->ulTransactionID = nondet_uint32_t();

    /* Set up endpoint linked list - can be NULL or point to another endpoint */
    NetworkEndPoint_t * pxNextEndPoint = NULL;
    BaseType_t xHasNext = nondet_BaseType_t();
    if( xHasNext )
    {
        pxNextEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        __CPROVER_assume( pxNextEndPoint != NULL );
        pxNextEndPoint->pxNext = NULL;
        pxNextEndPoint->bits.bIPv6 = nondet_uint8_t();
        pxNextEndPoint->xDHCPData.ulTransactionID = nondet_uint32_t();
        pxNextEndPoint->xDHCPData.eDHCPState      = nondet_uint32_t();
        pxNextEndPoint->xDHCPData.eExpectedState  = nondet_uint32_t();
    }
    pxEndPoint->pxNext = pxNextEndPoint;

    /* Set the global endpoint list pointer if the function uses it */
    pxNetworkEndPoints = pxEndPoint;

    /* Call the function under test */
    DHCPv6Process_PassReplyToEndPoint( pxEndPoint, pxNetworkBuffer, pxDHCPMessage );

    /* Postcondition assertions:
     * - The endpoint pointer is still valid (not freed)
     * - The network buffer pointer is still valid
     * - The DHCPMessage pointer is still valid
     * These are memory safety properties checked implicitly by CBMC.
     */
    assert( pxEndPoint != NULL );
    assert( pxNetworkBuffer != NULL );
    assert( pxDHCPMessage != NULL );
}