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
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_ND.h"

/* CBMC includes. */
#include "cbmc.h"

extern NDCacheRow_t xNDCache[ ipconfigND_CACHE_ENTRIES ];

/* The checksum generation is stubbed out since the actual checksum
 * does not matter. The stub will return an indeterminate value each time. */
uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                     size_t uxBufferLength,
                                     BaseType_t xOutgoingPacket )
{
    uint16_t usReturn;

    __CPROVER_assert( pucEthernetBuffer != NULL, "The ethernet buffer cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pucEthernetBuffer, uxBufferLength ), "pucEthernetBuffer should be readable." );

    /* Return an indeterminate value. */
    return usReturn;
}

/* This function has been tested separately. Therefore, we assume that the implementation is correct. */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
}

/* This function has been tested separately. Therefore, we assume that the implementation is correct. */
void vReceiveNA( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
}

/* This function has been tested separately. Therefore, we assume that the implementation is correct. */
void vReceiveRA( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
}

/* This function has been tested separately. Therefore, we assume that the implementation is correct. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxEvent != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assume( xReturn == pdPASS || xReturn == pdFAIL );

    return xReturn;
}

/* This is an output function implemented by a third party and
 * need not be tested with this proof. */
BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxDescriptor != NULL, "The network interface cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
    return 0;
}

/* Abstraction of this function creates and returns an endpoint; real endpoint doesn't matter in this test. */
NetworkEndPoint_t * FreeRTOS_InterfaceEPInSameSubnet_IPv6( const NetworkInterface_t * pxInterface,
                                                           const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndPoints = NULL;

    __CPROVER_assert( pxIPAddress != NULL, "The pxIPAddress cannot be NULL." );

    pxEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    if( ( pxEndPoints != NULL ) && __CPROVER_w_ok( pxEndPoints, sizeof( NetworkEndPoint_t ) ) )
    {
        /* Interface init. */
        pxEndPoints->pxNetworkInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );
        __CPROVER_assume( pxEndPoints->pxNetworkInterface != NULL );

        pxEndPoints->pxNext = NULL;
        pxEndPoints->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;
    }

    return pxEndPoints;
}

size_t uxIPHeaderSizePacket( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    size_t uxResult;

    __CPROVER_assume( ( uxResult == ipSIZE_OF_IPv4_HEADER ) || ( uxResult == ipSIZE_OF_IPv6_HEADER ) );

    return uxResult;
}

/* Stub for FreeRTOS_FindEndPointOnIP_IPv6 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndPoint = NULL;

    __CPROVER_assert( pxIPAddress != NULL, "The pxIPAddress cannot be NULL." );

    pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    if( ( pxEndPoint != NULL ) && __CPROVER_w_ok( pxEndPoint, sizeof( NetworkEndPoint_t ) ) )
    {
        pxEndPoint->pxNetworkInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );
        __CPROVER_assume( pxEndPoint->pxNetworkInterface != NULL );
        pxEndPoint->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;
        pxEndPoint->pxNext = NULL;
    }

    return pxEndPoint;
}

void ProcessICMPMessage_IPv6_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint32_t ulLen;
    NetworkBufferDescriptor_t *