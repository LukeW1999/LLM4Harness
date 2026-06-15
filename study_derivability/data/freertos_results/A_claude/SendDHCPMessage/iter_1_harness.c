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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"

/* CBMC includes. */
#include "cbmc.h"

/* Stub for FreeRTOS_GetAddressConfiguration if needed */
NetworkEndPoint_t * pxNetworkEndPoints = NULL;

/* Harness for SendDHCPMessage */
void SendDHCPMessage_harness( void )
{
    NetworkEndPoint_t * pxEndPoint;

    /* Allocate a nondeterministic endpoint */
    pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* Nondeterministically initialize the endpoint fields */
    pxEndPoint->xDHCPData.eDHCPState = nondet_uint32();
    pxEndPoint->xDHCPData.xDHCPTxTime = nondet_uint32();
    pxEndPoint->xDHCPData.xDHCPTxPeriod = nondet_uint32();
    pxEndPoint->xDHCPData.ulOfferedIPAddress = nondet_uint32();
    pxEndPoint->xDHCPData.ulDHCPServerAddress = nondet_uint32();
    pxEndPoint->xDHCPData.xUseBroadcast = nondet_BaseType_t();
    pxEndPoint->xDHCPData.ulLeaseTime = nondet_uint32();
    pxEndPoint->xDHCPData.xDHCPSocket = NULL;
    pxEndPoint->xDHCPData.ulTransactionId = nondet_uint32();
    pxEndPoint->xDHCPData.xDHCPTxTime = nondet_TickType_t();

    /* Constrain the DHCP state to valid values */
    __CPROVER_assume(
        ( pxEndPoint->xDHCPData.eDHCPState == eWaitingSendFirstDiscover ) ||
        ( pxEndPoint->xDHCPData.eDHCPState == eWaitingOffer ) ||
        ( pxEndPoint->xDHCPData.eDHCPState == eSendDHCPRequest ) ||
        ( pxEndPoint->xDHCPData.eDHCPState == eWaitingAcknowledge ) ||
        ( pxEndPoint->xDHCPData.eDHCPState == eLeasedAddress ) ||
        ( pxEndPoint->xDHCPData.eDHCPState == eNotUsingLeasedAddress )
    );

    /* Initialize MAC address nondeterministically */
    for( size_t i = 0; i < ipMAC_ADDRESS_LENGTH_BYTES; i++ )
    {
        pxEndPoint->xMACAddress.ucBytes[ i ] = nondet_uint8();
    }

    /* Initialize IP address fields */
    pxEndPoint->ipv4_settings.ulIPAddress = nondet_uint32();
    pxEndPoint->ipv4_settings.ulNetMask = nondet_uint32();
    pxEndPoint->ipv4_settings.ulGatewayAddress = nondet_uint32();
    pxEndPoint->ipv4_settings.ulDNSServerAddresses[ 0 ] = nondet_uint32();

    pxEndPoint->ipv4_defaults.ulIPAddress = nondet_uint32();
    pxEndPoint->ipv4_defaults.ulNetMask = nondet_uint32();
    pxEndPoint->ipv4_defaults.ulGatewayAddress = nondet_uint32();
    pxEndPoint->ipv4_defaults.ulDNSServerAddresses[ 0 ] = nondet_uint32();

    /* Set up linked list pointer */
    pxEndPoint->pxNext = NULL;

    /* Assign to global endpoint list */
    pxNetworkEndPoints = pxEndPoint;

    /* Call the function under test */
    SendDHCPMessage( pxEndPoint );

    /* Postconditions: verify no memory corruption occurred.
     * The function should not crash and the endpoint state should be valid. */
    assert( pxEndPoint != NULL );
}