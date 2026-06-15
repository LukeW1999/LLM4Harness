/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"

/* CBMC includes. */
#include "cbmc.h"

/* Stub declarations. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks );
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer );
eARPLookupResult_t eARPGetCacheEntry( uint32_t * pulIPAddress,
                                      MACAddress_t * const pxMACAddress );
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                    BaseType_t xReleaseAfterSend );
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout );

/* The function under test. */
void vProcessGeneratedUDPPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );

void vProcessGeneratedUDPPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t bufferSize;

    /* Allocate a network buffer descriptor nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* We allow NULL to test NULL-pointer handling. */
    if( pxNetworkBuffer == NULL )
    {
        /* Call with NULL buffer - function should handle gracefully. */
        vProcessGeneratedUDPPacket( NULL );
        return;
    }

    /* The buffer must be large enough to hold a UDP packet:
     * Ethernet header + IP header + UDP header + some payload. */
    bufferSize = sizeof( UDPPacket_t ) + nondet_uint32();
    __CPROVER_assume( bufferSize >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( bufferSize <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( bufferSize );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = bufferSize;
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( pxNetworkBuffer->xDataLength <= bufferSize );

    /* Set nondeterministic destination IP address. */
    pxNetworkBuffer->ulIPAddress = nondet_uint32();

    /* Set nondeterministic source and destination ports. */
    pxNetworkBuffer->usPort = nondet_uint16();
    pxNetworkBuffer->usBoundPort = nondet_uint16();

    /* Socket pointer can be NULL or non-NULL. */
    /* We leave it as uninitialized/nondeterministic in the buffer. */

    /* The interface pointer. */
    NetworkInterface_t * pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    pxNetworkBuffer->pxInterface = pxInterface;

    /* The endpoint pointer. */
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    if( pxEndPoint != NULL )
    {
        /* Set nondeterministic endpoint IP address. */
        pxEndPoint->ipv4_settings.ulIPAddress = nondet_uint32();
        pxEndPoint->ipv4_settings.ulNetMask   = nondet_uint32();
        pxEndPoint->ipv4_settings.ulGatewayAddress = nondet_uint32();
        pxEndPoint->ipv4_settings.ulDNSServerAddresses[ 0 ] = nondet_uint32();
        pxEndPoint->pxNext = NULL;

        if( pxInterface != NULL )
        {
            pxEndPoint->pxNetworkInterface = pxInterface;
        }
    }
    pxNetworkBuffer->pxEndPoint = pxEndPoint;

    /* Initialize the Ethernet buffer with nondeterministic data. */
    /* The UDPPacket_t overlay will be used inside the function. */
    UDPPacket_t * pxUDPPacket = ( UDPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Set nondeterministic IP header fields. */
    pxUDPPacket->xIPHeader.ulDestinationIPAddress = nondet_uint32();
    pxUDPPacket->xIPHeader.ulSourceIPAddress      = nondet_uint32();
    pxUDPPacket->xIPHeader.ucVersionHeaderLength  = nondet_uint8();
    pxUDPPacket->xIPHeader.ucTimeToLive           = ipconfigUDP_TIME_TO_LIVE;
    pxUDPPacket->xIPHeader.usLength               = nondet_uint16();
    pxUDPPacket->xIPHeader.ucProtocol             = ipPROTOCOL_UDP;

    /* Set nondeterministic UDP header fields. */
    pxUDPPacket->xUDPHeader.usDestinationPort = nondet_uint16();
    pxUDPPacket->xUDPHeader.usSourcePort      = nondet_uint16();
    pxUDPPacket->xUDPHeader.usLength          = nondet_uint16();

    /* Call the function under test. */
    vProcessGeneratedUDPPacket( pxNetworkBuffer );

    /* No specific postconditions to assert beyond no memory safety violations,
     * which CBMC checks automatically. */
}