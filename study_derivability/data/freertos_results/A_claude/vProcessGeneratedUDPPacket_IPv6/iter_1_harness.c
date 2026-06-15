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
#include "FreeRTOS_ND.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"

/* CBMC proof harness for vProcessGeneratedUDPPacket_IPv6 */

/* Stub declarations for functions called internally */
extern void vProcessGeneratedUDPPacket_IPv6( NetworkBufferDescriptor_t * pxNetworkBuffer );

/* NetworkInterface send stub */
BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend );

void vProcessGeneratedUDPPacket_IPv6_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    UDPPacket_IPv6_t * pxUDPPacket;
    size_t xPayloadSize;
    uint8_t * pucEthernetBuffer;

    /* Allocate a network buffer descriptor */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate the Ethernet buffer with enough space for IPv6 UDP packet */
    /* Minimum size: Ethernet header + IPv6 header + UDP header */
    size_t xBufferSize;
    __CPROVER_assume( xBufferSize >= sizeof( UDPPacket_IPv6_t ) );
    __CPROVER_assume( xBufferSize <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Set up the network buffer */
    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = xBufferSize;

    /* Set up the UDP packet structure */
    pxUDPPacket = ( UDPPacket_IPv6_t * ) pucEthernetBuffer;

    /* Set up the IPv6 header with nondeterministic values */
    /* Source and destination IPv6 addresses */
    __CPROVER_havoc_object( &pxUDPPacket->xIPHeader );
    __CPROVER_havoc_object( &pxUDPPacket->xUDPHeader );
    __CPROVER_havoc_object( &pxUDPPacket->xEthernetHeader );

    /* Set next header to UDP */
    pxUDPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_UDP;

    /* Set version to 6 */
    pxUDPPacket->xIPHeader.ucVersionTrafficClass = 0x60;

    /* Set payload length */
    uint16_t usPayloadLength;
    __CPROVER_assume( usPayloadLength >= sizeof( UDPHeader_t ) );
    __CPROVER_assume( usPayloadLength <= ( uint16_t )( xBufferSize - sizeof( IPHeader_IPv6_t ) - ipSIZE_OF_ETH_HEADER ) );
    pxUDPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( usPayloadLength );

    /* Set up socket address info in the network buffer */
    pxNetworkBuffer->usPort = ( uint16_t ) nondet_uint16();
    pxNetworkBuffer->usBoundPort = ( uint16_t ) nondet_uint16();

    /* Set up the interface - may be NULL or valid */
    NetworkInterface_t * pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );

    if( pxInterface != NULL )
    {
        NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

        if( pxEndPoint != NULL )
        {
            __CPROVER_havoc_object( pxEndPoint );
            pxEndPoint->pxNetworkInterface = pxInterface;
            pxInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;
            pxEndPoint->pxNext = NULL;
        }

        pxInterface->pxEndPoint = pxEndPoint;
        pxInterface->pxNext = NULL;
        pxNetworkBuffer->pxInterface = pxInterface;
        pxNetworkBuffer->pxEndPoint = pxEndPoint;
    }
    else
    {
        pxNetworkBuffer->pxInterface = NULL;
        pxNetworkBuffer->pxEndPoint = NULL;
    }

    /* Set xDataLength to a valid value */
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( UDPPacket_IPv6_t ) );
    __CPROVER_assume( pxNetworkBuffer->xDataLength <= xBufferSize );

    /* Call the function under test */
    vProcessGeneratedUDPPacket_IPv6( pxNetworkBuffer );
}

/* Stub for NetworkInterfaceOutputFunction */
BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend )
{
    /* Non-deterministic return value */
    BaseType_t xReturn;
    return xReturn;
}