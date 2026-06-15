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
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"

#include "cbmc.h"

/* Global endpoint list used by stubs */
NetworkEndPoint_t * pxNetworkEndPoints;
NetworkInterface_t * pxNetworkInterfaces;

/* Abstraction of xIsCallingFromIPTask */
BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Abstraction of vReleaseNetworkBufferAndDescriptor */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "vReleaseNetworkBufferAndDescriptor: pxNetworkBuffer != NULL" );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL,
                      "vReleaseNetworkBufferAndDescriptor: pxNetworkBuffer->pucEthernetBuffer != NULL" );

    free( pxNetworkBuffer->pucEthernetBuffer );
    free( pxNetworkBuffer );
}

/* Abstraction of xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    __CPROVER_assert( pxEvent != NULL, "xSendEventStructToIPTask: pxEvent != NULL" );

    BaseType_t xReturn;
    return xReturn;
}

/* Abstraction of eConsiderFrameForProcessing */
eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer )
{
    __CPROVER_assert( pucEthernetBuffer != NULL,
                      "eConsiderFrameForProcessing: pucEthernetBuffer != NULL" );

    eFrameProcessingResult_t eReturn;
    return eReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnMAC */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    __CPROVER_assert( pxMACAddress != NULL,
                      "FreeRTOS_FindEndPointOnMAC: pxMACAddress != NULL" );

    NetworkEndPoint_t * pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    return pxEndPoint;
}

/* Abstraction of FreeRTOS_FindEndPointOnNetMask */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress,
                                                    uint32_t ulCaller )
{
    NetworkEndPoint_t * pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );
    return pxEndPoint;
}

/* Abstraction of xCheckRequiresARPResolution */
BaseType_t xCheckRequiresARPResolution( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "xCheckRequiresARPResolution: pxNetworkBuffer != NULL" );

    BaseType_t xReturn;
    return xReturn;
}

/* Abstraction of vARPRefreshCacheEntry */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress,
                            NetworkEndPoint_t * pxEndPoint )
{
    /* Nothing to do. */
}

/* Abstraction of xNetworkInterfaceOutput */
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                    BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "xNetworkInterfaceOutput: pxNetworkBuffer != NULL" );

    BaseType_t xReturn;
    return xReturn;
}

/*
 * Harness for ProcessEthernetPacket.
 * ProcessEthernetPacket takes a NetworkBufferDescriptor_t * and processes
 * the Ethernet frame it contains.
 */
void ProcessEthernetPacket_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxBufferLength;

    /* Allocate a network buffer descriptor */
    pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );

    /* Assume we always have a valid network buffer */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Assume a reasonable buffer size: at least an Ethernet header, less than MTU */
    __CPROVER_assume( uxBufferLength >= sizeof( EthernetHeader_t ) &&
                      uxBufferLength <= ipconfigNETWORK_MTU );

    pxNetworkBuffer->xDataLength = uxBufferLength;

    /* Allocate the Ethernet buffer */
    pxNetworkBuffer->pucEthernetBuffer = safeMalloc( uxBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* Set up a network interface (can be NULL or non-NULL) */
    pxNetworkBuffer->pxInterface = safeMalloc( sizeof( NetworkInterface_t ) );

    /* Set up an endpoint (can be NULL or non-NULL) */
    pxNetworkBuffer->pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    /* If endpoint is non-NULL, initialise its bits field to avoid uninitialised reads */
    if( pxNetworkBuffer->pxEndPoint != NULL )
    {
        pxNetworkBuffer->pxEndPoint->bits.bIPv6 = 0;
    }

    /* Call the function under verification */
    ProcessEthernetPacket( pxNetworkBuffer );
}