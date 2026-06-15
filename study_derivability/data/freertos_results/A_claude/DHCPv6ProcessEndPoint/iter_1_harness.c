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
#include "FreeRTOS_DHCPv6.h"
#include "FreeRTOS_Sockets.h"

/* CBMC proof includes */
#include "cbmc.h"

/* Extern declarations for DHCPv6 internal state if needed */
extern void DHCPv6ProcessEndPoint( BaseType_t xReset,
                                   NetworkEndPoint_t * pxEndPoint,
                                   DHCPMessage_IPv6_t * pxDHCPMessage );

/* Helper to allocate a nondeterministic IPv6 address */
static void initialise_IPv6_address( IPv6_Address_t * pxAddress )
{
    __CPROVER_assert( pxAddress != NULL, "pxAddress must not be NULL" );
    /* Fill with nondeterministic bytes */
    for( size_t i = 0; i < ipSIZE_OF_IPv6_ADDRESS; i++ )
    {
        pxAddress->ucBytes[ i ] = nondet_uint8_t();
    }
}

/* Helper to create and initialise a NetworkEndPoint_t */
static NetworkEndPoint_t * allocate_and_init_endpoint( void )
{
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    __CPROVER_assume( pxEndPoint != NULL );

    /* Initialise DHCPData */
    pxEndPoint->xDHCPData.eDHCPState       = ( eDHCPState_t ) nondet_uint32_t();
    pxEndPoint->xDHCPData.eExpectedState    = ( eDHCPState_t ) nondet_uint32_t();
    pxEndPoint->xDHCPData.xDHCPSocket       = NULL;
    pxEndPoint->xDHCPData.ulTransactionId   = nondet_uint32_t();
    pxEndPoint->xDHCPData.xDHCPTxTime       = nondet_uint32_t();
    pxEndPoint->xDHCPData.xDHCPTxPeriod     = nondet_uint32_t();
    pxEndPoint->xDHCPData.ulOfferedIPAddress = nondet_uint32_t();
    pxEndPoint->xDHCPData.ulDHCPServerAddress = nondet_uint32_t();
    pxEndPoint->xDHCPData.xUseBroadcast     = nondet_BaseType_t();
    pxEndPoint->xDHCPData.ulLeaseTime       = nondet_uint32_t();

    /* Constrain eDHCPState to valid range */
    __CPROVER_assume( pxEndPoint->xDHCPData.eDHCPState <= eLeasedAddress );
    __CPROVER_assume( pxEndPoint->xDHCPData.eExpectedState <= eLeasedAddress );

    /* Initialise IPv6 specific DHCPv6 data */
    initialise_IPv6_address( &( pxEndPoint->ipv6_settings.xIPAddress ) );
    initialise_IPv6_address( &( pxEndPoint->ipv6_settings.xPrefix ) );
    pxEndPoint->ipv6_settings.uxPrefixLength = nondet_uint8_t();
    initialise_IPv6_address( &( pxEndPoint->ipv6_settings.xGatewayAddress ) );
    initialise_IPv6_address( &( pxEndPoint->ipv6_settings.xDNSServerAddresses[ 0 ] ) );
    initialise_IPv6_address( &( pxEndPoint->ipv6_settings.xDNSServerAddresses[ 1 ] ) );

    /* Bits field */
    pxEndPoint->bits.bIPv6 = pdTRUE_UNSIGNED;
    pxEndPoint->bits.bWantDHCP = nondet_uint32_t() & 1U;
    pxEndPoint->bits.bCallDownHook = nondet_uint32_t() & 1U;
    pxEndPoint->bits.bIPInitialised = nondet_uint32_t() & 1U;

    /* Network interface pointer - can be NULL or valid */
    pxEndPoint->pxNetworkInterface = NULL;

    /* Next endpoint pointer */
    pxEndPoint->pxNext = NULL;

    /* DHCPv6 specific fields */
    pxEndPoint->xDHCPData.xDHCPSocket = NULL;

    return pxEndPoint;
}

/* Helper to create and initialise a DHCPMessage_IPv6_t */
static DHCPMessage_IPv6_t * allocate_and_init_dhcp_message( void )
{
    DHCPMessage_IPv6_t * pxDHCPMessage;
    BaseType_t xHasMessage = nondet_BaseType_t();

    /* Message may be NULL */
    if( xHasMessage == pdFALSE )
    {
        return NULL;
    }

    pxDHCPMessage = ( DHCPMessage_IPv6_t * ) malloc( sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( pxDHCPMessage != NULL );

    /* Fill message type */
    pxDHCPMessage->uxMessageType = nondet_uint8_t();

    /* Fill transaction ID */
    pxDHCPMessage->ulTransactionID = nondet_uint32_t();

    /* Fill addresses */
    initialise_IPv6_address( &( pxDHCPMessage->xIPAddress ) );
    initialise_IPv6_address( &( pxDHCPMessage->xDNSServers[ 0 ] ) );
    initialise_IPv6_address( &( pxDHCPMessage->xDNSServers[ 1 ] ) );
    initialise_IPv6_address( &( pxDHCPMessage->xServerID.pucID ) );

    /* Fill prefix info */
    pxDHCPMessage->ucprefixLength = nondet_uint8_t();
    initialise_IPv6_address( &( pxDHCPMessage->xPrefixAddress ) );

    /* Fill timing parameters */
    pxDHCPMessage->ulPreferredLifeTime = nondet_uint32_t();
    pxDHCPMessage->ulValidLifeTime     = nondet_uint32_t();