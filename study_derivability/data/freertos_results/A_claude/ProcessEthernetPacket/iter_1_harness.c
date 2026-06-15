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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* CBMC proof helpers. */
#include "cbmc.h"

/* The minimum ethernet frame size including the ethernet header. */
#define MINIMUM_ETHERNET_FRAME_SIZE    ( sizeof( EthernetHeader_t ) )

/* Maximum ethernet frame payload size. */
#define ETHERNET_FRAME_PAYLOAD_MAX     ( ipconfigNETWORK_MTU + sizeof( EthernetHeader_t ) )

/*
 * Stub for vReleaseNetworkBufferAndDescriptor to prevent leaks being flagged.
 */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    /* Stub: do nothing */
    ( void ) pxNetworkBuffer;
}

/*
 * Stub for xSendEventStructToIPTask.
 */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    /* Return a nondeterministic value */
    BaseType_t xReturn;
    return xReturn;
}

/*
 * Stub for xIsCallingFromIPTask.
 */
BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    return xReturn;
}

/*
 * Stub for FreeRTOS_FindEndPointOnMAC.
 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    static NetworkEndPoint_t xEndPoint;
    NetworkEndPoint_t * pxReturn;

    if( nondet_bool() )
    {
        pxReturn = &xEndPoint;
    }
    else
    {
        pxReturn = NULL;
    }

    return pxReturn;
}

/*
 * Stub for eARPProcessPacket.
 */
eFrameProcessingResult_t eARPProcessPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t eReturn;
    __CPROVER_assume( eReturn == eReleaseBuffer ||
                      eReturn == eProcessBuffer ||
                      eReturn == eReturnEthernetFrame ||
                      eReturn == eFrameConsumed );
    return eReturn;
}

/*
 * Stub for eNDGetCacheEntry (IPv6 Neighbor Discovery).
 */
#if defined( ipconfigUSE_IPv6 ) && ( ipconfigUSE_IPv6 != 0 )
eARPLookupResult_t eNDGetCacheEntry( IPv6_Address_t * pxIPAddress,
                                     MACAddress_t * const pxMACAddress,
                                     struct xNetworkEndPoint ** ppxEndPoint )
{
    eARPLookupResult_t eReturn;
    return eReturn;
}
#endif

/*
 * Stub for xCheckRequiresARPResolution.
 */
BaseType_t xCheckRequiresARPResolution( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    BaseType_t xReturn;
    return xReturn;
}

/*
 * Stub for vReturnEthernetFrame.
 */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    ( void ) pxNetworkBuffer;
    ( void ) xReleaseAfterSend;
}

/*
 * Stub for xNetworkInterfaceOutput.
 */
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                    BaseType_t xReleaseAfterSend )
{
    BaseType_t xReturn;
    return xReturn;
}

/*
 * Stub for prvProcessIPPacket.
 */
eFrameProcessingResult_t prvProcessIPPacket( IPPacket_t * pxIPPacket,
                                             NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t eReturn;
    __CPROVER_assume( eReturn == eReleaseBuffer ||
                      eReturn == eProcessBuffer ||
                      eReturn == eReturnEthernetFrame ||
                      eReturn == eFrameConsumed );
    return eReturn;
}

/*
 * Stub for pxDuplicateNetworkBufferWithDescriptor.
 */
NetworkBufferDescriptor_t * pxDuplicateNetworkBufferWithDescriptor(
    const NetworkBufferDescriptor_t * const pxNetworkBuffer,
    size_t xNewLength )
{
    NetworkBufferDescriptor_t * pxReturn;

    if( nondet_bool() )
    {
        static NetworkBufferDescriptor_t xDupBuffer;
        static uint8_t ucDupData[ ETHERNET_FRAME_PAYLOAD_MAX ];
        xDupBuffer.pucEthernetBuffer = ucDupData;
        xDupBuffer.xDataLength = xNewLength;
        pxReturn = &xDupBuffer;
    }
    else
    {
        pxReturn = NULL;
    }

    return pxReturn;
}

/*
 * Stub for xSendEventToIPTask.
 */
BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    BaseType_t xReturn;
    return xReturn;
}

/*
 * Harness for ProcessEthernetPacket.
 */
void ProcessEthernetPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xDataLength;
    NetworkInterface_t * pxInterface;
    NetworkEndPoint_t * pxEndPoint;

    /* Allocate a network buffer descriptor nondeterministically. */
    pxNetwork