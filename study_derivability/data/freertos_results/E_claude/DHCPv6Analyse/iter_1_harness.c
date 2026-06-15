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
#include <stdlib.h>

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

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
BaseType_t __CPROVER_file_local_FreeRTOS_DHCPv6_c_prvDHCPv6Analyse( struct xNetworkEndPoint * pxEndPoint,
                                                                     const uint8_t * pucAnswer,
                                                                     size_t uxTotalLength,
                                                                     DHCPMessage_IPv6_t * pxDHCPMessage );

/*
 * The maximum length of the DHCPv6 packet buffer used in the proof.
 * This bound is chosen to keep the proof tractable while covering
 * realistic packet sizes.
 */
#ifndef DHCPV6_ANALYSE_MAX_PACKET_LEN
    #define DHCPV6_ANALYSE_MAX_PACKET_LEN    1024U
#endif

void DHCPv6Analyse_harness()
{
    /* Create a non-deterministic endpoint. */
    NetworkEndPoint_t * pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    /* The function under test requires a non-NULL endpoint. */
    __CPROVER_assume( pxEndPoint != NULL );

    /* Create a non-deterministic packet length that is bounded so the
     * proof remains tractable. */
    size_t uxTotalLength;
    __CPROVER_assume( uxTotalLength <= DHCPV6_ANALYSE_MAX_PACKET_LEN );

    /* Allocate a buffer of the chosen length.  Allow uxTotalLength == 0
     * so that boundary conditions at the start of the packet are exercised.
     * When uxTotalLength is zero, pucAnswer may be NULL – the function is
     * expected to handle that gracefully. */
    uint8_t * pucAnswer = NULL;

    if( uxTotalLength > 0U )
    {
        pucAnswer = safeMalloc( uxTotalLength );
        /* Allow the malloc to fail (pucAnswer == NULL) so that the function
         * handles a NULL buffer correctly. */
    }

    /* Create a non-deterministic DHCPMessage_IPv6_t.  The function writes
     * parsed data into this structure, so it must be non-NULL per the
     * preconditions established in the example harness for the outer
     * vDHCPv6Process proof. */
    DHCPMessage_IPv6_t * pxDHCPMessage = safeMalloc( sizeof( DHCPMessage_IPv6_t ) );
    __CPROVER_assume( pxDHCPMessage != NULL );

    /* Call the function under test. */
    ( void ) __CPROVER_file_local_FreeRTOS_DHCPv6_c_prvDHCPv6Analyse( pxEndPoint,
                                                                       pucAnswer,
                                                                       uxTotalLength,
                                                                       pxDHCPMessage );
}