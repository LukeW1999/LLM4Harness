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
#include "FreeRTOS_ICMP.h"

/* CBMC includes. */
#include "cbmc.h"

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

/* Abstraction of usGenerateChecksum */
uint16_t usGenerateChecksum( uint16_t usSum,
                             const uint8_t * pucNextData,
                             size_t uxByteCount )
{
    uint16_t usReturn;

    __CPROVER_assert( pucNextData != NULL, "The next data pointer cannot be NULL." );
    __CPROVER_assert( __CPROVER_r_ok( pucNextData, uxByteCount ), "pucNextData should be readable." );

    return usReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnIP_IPv6 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndPoint = NULL;

    __CPROVER_assert( pxIPAddress != NULL, "The IP address cannot be NULL." );

    if( nondet_bool() )
    {
        pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

        if( pxEndPoint != NULL )
        {
            pxEndPoint->pxNetworkInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );

            if( pxEndPoint->pxNetworkInterface != NULL )
            {
                pxEndPoint->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;
            }

            pxEndPoint->pxNext = NULL;
        }
    }

    return pxEndPoint;
}

/* Abstraction of FreeRTOS_InterfaceEndPointOnNetMask_IPv6 */
NetworkEndPoint_t * FreeRTOS_InterfaceEndPointOnNetMask_IPv6( const NetworkInterface_t * pxInterface,
                                                              const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndPoint = NULL;

    __CPROVER_assert( pxInterface != NULL, "The network interface cannot be NULL." );
    __CPROVER_assert( pxIPAddress != NULL, "The IP address cannot be NULL." );

    if( nondet_bool() )
    {
        pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

        if( pxEndPoint != NULL )
        {
            pxEndPoint->pxNetworkInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );

            if( pxEndPoint->pxNetworkInterface != NULL )
            {
                pxEndPoint->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;
            }

            pxEndPoint->pxNext = NULL;
        }
    }

    return pxEndPoint;
}

void harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxICMPSize;
    size_t uxNeededSize;

    /* uxICMPSize must be large enough to hold the ICMPv6 echo reply structure
     * plus the Ethernet and IPv6 headers. Bound it to avoid CBMC object size issues. */
    __CPROVER_assume( uxICMPSize >= sizeof( ICMPPacket_IPv6_t ) );
    __CPROVER_assume( uxICMPSize < ( CBMC_MAX_OBJECT_SIZE - ipBUFFER_PADDING ) );

    pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxICMPSize, 0 );

    /* The function under test expects a valid network buffer. */
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* Havoc the ethernet buffer to cover all possible packet contents. */
    __CPROVER_havoc_slice( pxNetworkBuffer->pucEthernetBuffer, uxICMPSize );

    pxNetworkBuffer->xDataLength = uxICMPSize;

    /* Add an endpoint to the network buffer. */
    pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t