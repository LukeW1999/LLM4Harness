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

#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_ICMP.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/* Stub for xIsCallingFromIPTask if needed */
BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Stub for xSendEventStructToIPTask if needed */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Stub for FreeRTOS_FindEndPointOnNetMask if needed */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress,
                                                    uint32_t ulCaller )
{
    NetworkEndPoint_t * pxEndPoint;
    return pxEndPoint;
}

/* Stub for FreeRTOS_FindEndPointOnIP_IPv4 if needed */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv4( uint32_t ulIPAddress,
                                                    uint32_t ulCaller )
{
    NetworkEndPoint_t * pxEndPoint;
    return pxEndPoint;
}

void ProcessICMPEchoRequest_harness()
{
    /* Allocate a network buffer descriptor */
    NetworkBufferDescriptor_t * pxNetworkBuffer =
        safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate an ICMP packet to back the ethernet buffer */
    ICMPPacket_t * pxICMPPacket = safeMalloc( sizeof( ICMPPacket_t ) );

    __CPROVER_assume( pxICMPPacket != NULL );

    /* Set up the network buffer descriptor */
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) pxICMPPacket;
    pxNetworkBuffer->xDataLength = sizeof( ICMPPacket_t );

    /* Optionally attach an endpoint */
    NetworkEndPoint_t * pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    pxNetworkBuffer->pxEndPoint = pxEndPoint;

    /* Call the function under verification */
    __CPROVER_file_local_FreeRTOS_ICMP_c_prvProcessICMPEchoRequest(
        pxICMPPacket,
        pxNetworkBuffer );
}