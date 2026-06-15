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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

#include "cbmc.h"

/* Abstraction of FreeRTOS_FindEndPointOnMAC */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    NetworkEndPoint_t * pxEndPoint;

    __CPROVER_assert( pxMACAddress != NULL, "pxMACAddress != NULL" );

    /* pxInterface can be NULL. */

    pxEndPoint = safeMalloc( sizeof( NetworkEndPoint_t ) );

    return pxEndPoint;
}

/* Abstraction of FreeRTOS_OutputARPRequest */
void FreeRTOS_OutputARPRequest( uint32_t ulIPAddress )
{
    /* No implementation needed for this proof */
}

/* Abstraction of FreeRTOS_OutputGenerator */
void FreeRTOS_OutputGenerator( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                NetworkEndPoint_t * pxEndPoint )
{
    /* No implementation needed for this proof */
}

/* Abstraction of FreeRTOS_NetworkDown */
void FreeRTOS_NetworkDown( NetworkInterface_t * pxInterface )
{
    /* No implementation needed for this proof */
}

/* Abstraction of xIsIPFrameForUs */
BaseType_t xIsIPFrameForUs( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    BaseType_t xReturn;

    /* Return an unconstrained value */
    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of xProcessReceivedUDPPacket */
BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                       NetworkEndPoint_t * pxEndPoint )
{
    BaseType_t xReturn;

    /* Return an unconstrained value */
    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of xProcessReceivedTCPPacket */
BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                       NetworkEndPoint_t * pxEndPoint )
{
    BaseType_t xReturn;

    /* Return an unconstrained value */
    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of xProcessReceivedICMPPacket */
BaseType_t xProcessReceivedICMPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                        NetworkEndPoint_t * pxEndPoint )
{
    BaseType_t xReturn;

    /* Return an unconstrained value */
    __CPROVER_assume( xReturn == pdTRUE || xReturn == pdFALSE );

    return xReturn;
}

/* Abstraction of vReturnEthernetFrame */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    /* No implementation needed for this proof */
}

/* The harness test proceeds to call ProcessEthernetPacket with an unconstrained buffer */
void ProcessEthernetPacket_harness()
{
    BaseType_t uBuffSize;

    /* Assume minimum and maximum buffer size expected */
    __CPROVER_assume( uBuffSize >= sizeof( EthernetHeader_t ) && uBuffSize < ipconfigNETWORK_MTU );
    uint8_t * const pucEthernetBuffer = safeMalloc( uBuffSize );

    NetworkBufferDescriptor_t * pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = uBuffSize;

    ProcessEthernetPacket( pxNetworkBuffer );
}