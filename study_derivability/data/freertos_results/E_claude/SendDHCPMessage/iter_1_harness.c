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

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
void __CPROVER_file_local_FreeRTOS_DHCP_c_SendDHCPMessage( NetworkEndPoint_t * pxEndPoint );

/* Stub for FreeRTOS_sendto so we don't need a real socket. */
int32_t FreeRTOS_sendto( Socket_t xSocket,
                         const void * pvBuffer,
                         size_t uxTotalDataLength,
                         BaseType_t xFlags,
                         const struct freertos_sockaddr * pxDestinationAddress,
                         socklen_t xDestinationAddressLength )
{
    int32_t xReturn;
    return xReturn;
}

/* Stub for FreeRTOS_socket. */
Socket_t FreeRTOS_socket( BaseType_t xDomain,
                          BaseType_t xType,
                          BaseType_t xProtocol )
{
    Socket_t xSocket;
    return xSocket;
}

/* Stub for FreeRTOS_setsockopt. */
BaseType_t FreeRTOS_setsockopt( Socket_t xSocket,
                                int32_t lLevel,
                                int32_t lOptionName,
                                const void * pvOptionValue,
                                size_t uxOptionLength )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Stub for FreeRTOS_bind. */
BaseType_t FreeRTOS_bind( Socket_t xSocket,
                          struct freertos_sockaddr * pxAddress,
                          socklen_t xAddressLength )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Stub for vSocketClose / FreeRTOS_closesocket. */
BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Stub for xTaskGetTickCount. */
TickType_t xTaskGetTickCount( void )
{
    TickType_t xTick;
    return xTick;
}

/* Stub for vPortEnterCritical / vPortExitCritical if needed. */

void harness()
{
    NetworkEndPoint_t * pxEndPoint;
    DHCPData_t * pxDHCPData;

    /* Allocate and assume non-null endpoint. */
    pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* The DHCP state data is embedded in the endpoint. Make it non-deterministic
     * but within valid ranges so CBMC can explore all paths. */

    /* Ensure the DHCP transaction ID and other fields are unconstrained. */
    /* xDHCPData is a field inside NetworkEndPoint_t. */
    /* We leave the entire struct non-deterministic (already the case after safeMalloc). */

    /* Constrain the eDHCPState to a valid enum range so the switch/if logic
     * inside SendDHCPMessage is exercised properly. */
    __CPROVER_assume( pxEndPoint->xDHCPData.eDHCPState <= eLeasedAddress );

    /* Ensure the xDHCPSocket field is either a valid-looking socket or
     * FREERTOS_INVALID_SOCKET to exercise both branches. */

    /* Call the function under test. */
    __CPROVER_file_local_FreeRTOS_DHCP_c_SendDHCPMessage( pxEndPoint );
}