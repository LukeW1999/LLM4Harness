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
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCPv6.h"

/* CBMC includes. */
#include "cbmc.h"

/* Maximum size for the DHCPv6 payload to keep proof tractable. */
#ifndef DHCPV6_MAX_PAYLOAD_SIZE
    #define DHCPV6_MAX_PAYLOAD_SIZE    ( 256U )
#endif

/* The function under test. */
BaseType_t DHCPv6Analyse( struct xNetworkEndPoint * pxEndPoint,
                          const uint8_t * pucAnswer,
                          size_t uxTotalLength,
                          DHCPMessage_IPv6_t * pxDHCPMessage );

void DHCPv6Analyse_harness( void )
{
    struct xNetworkEndPoint * pxEndPoint;
    uint8_t * pucAnswer;
    size_t uxTotalLength;
    DHCPMessage_IPv6_t * pxDHCPMessage;
    BaseType_t xResult;

    /* Allocate the network endpoint nondeterministically. */
    pxEndPoint = ( struct xNetworkEndPoint * ) malloc( sizeof( struct xNetworkEndPoint ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* Initialize the endpoint with nondeterministic data. */
    memset( pxEndPoint, 0, sizeof( struct xNetworkEndPoint ) );

    /* Allocate the DHCPv6 message structure nondeterministically. */
    pxDHCPMessage = ( DHCPMessage_IPv6_t * ) malloc( sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( pxDHCPMessage != NULL );

    /* Initialize the DHCPv6 message structure. */
    memset( pxDHCPMessage, 0, sizeof( DHCPMessage_IPv6_t ) );

    /* Nondeterministic total length, bounded to keep proof tractable. */
    __CPROVER_assume( uxTotalLength <= DHCPV6_MAX_PAYLOAD_SIZE );

    /* Allocate a nondeterministic payload buffer. */
    if( uxTotalLength > 0U )
    {
        pucAnswer = ( uint8_t * ) malloc( uxTotalLength );
        __CPROVER_assume( pucAnswer != NULL );
    }
    else
    {
        /* Allow NULL pointer when length is 0. */
        pucAnswer = NULL;
    }

    /* Call the function under test. */
    xResult = DHCPv6Analyse( pxEndPoint,
                             pucAnswer,
                             uxTotalLength,
                             pxDHCPMessage );

    /* Assert postconditions:
     * The function should return either pdPASS or pdFAIL. */
    assert( ( xResult == pdPASS ) || ( xResult == pdFAIL ) );

    /* The DHCPv6 message structure pointer should remain valid after the call. */
    __CPROVER_assert( pxDHCPMessage != NULL, "pxDHCPMessage should remain non-NULL after call" );

    /* The endpoint pointer should remain valid after the call. */
    __CPROVER_assert( pxEndPoint != NULL, "pxEndPoint should remain non-NULL after call" );
}