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

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_DHCPv6.h"
#include "FreeRTOS_BitConfig.h"

/* CBMC includes. */
#include "cbmc.h"

/* Declare the function under test. */
void __CPROVER_file_local_FreeRTOS_DHCPv6_c_DHCPv6Process_PassReplyToEndPoint( struct xNetworkEndPoint * pxEndPoint );

/* Mock for vDHCPv6ProcessEndPoint or any internal calls that send events. */
void vIPNetworkUpCalls( struct xNetworkEndPoint * pxEndPoint )
{
    __CPROVER_assert( __CPROVER_r_ok( pxEndPoint, sizeof( struct xNetworkEndPoint ) ),
                      "pxEndPoint region must be readable" );
}

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;
    return xReturn;
}

void FreeRTOS_ReleaseUDPPayloadBuffer( void const * pvBuffer )
{
}

void * pvPortMalloc( size_t xSize )
{
    return safeMalloc( xSize );
}

void vPortFree( void * pv )
{
    free( pv );
}

void harness()
{
    NetworkEndPoint_t * pxEndPoint;
    DHCPData_t * pxDHCPData;

    /* Allocate and initialise the endpoint. */
    pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* Zero-initialise to avoid any undefined behaviour from uninitialised reads. */
    ( void ) memset( pxEndPoint, 0, sizeof( NetworkEndPoint_t ) );

    /* The endpoint must have a valid DHCPData structure embedded within it.
     * In FreeRTOS+TCP the DHCPv6 state is stored inside the endpoint's
     * xDHCPData (or equivalent) field.  Provide non-deterministic but
     * structurally valid values so that the proof explores all reachable paths. */

    /* Provide a non-deterministic DHCPv6 message. */
    DHCPMessage_IPv6_t * pxDHCPMessage;
    pxDHCPMessage = safeMalloc( sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( pxDHCPMessage != NULL );
    ( void ) memset( pxDHCPMessage, 0, sizeof( DHCPMessage_IPv6_t ) );

    /* Attach the DHCPv6 message to the endpoint's DHCPv6 data. */
    pxEndPoint->xDHCPData.pxDHCPMessage = pxDHCPMessage;

    /* Allow the DHCPv6 state machine to be in any state. */
    /* pxEndPoint->xDHCPData.eDHCPState is left non-deterministic. */

    /* Call the function under test. */
    __CPROVER_file_local_FreeRTOS_DHCPv6_c_DHCPv6Process_PassReplyToEndPoint( pxEndPoint );
}