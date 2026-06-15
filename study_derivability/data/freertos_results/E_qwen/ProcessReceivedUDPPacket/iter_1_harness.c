#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "cbmc.h"

NetworkEndPoint_t xEndpoint;

BaseType_t xCheckRequiresResolution( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ), "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

void vARPRefreshCacheEntryAge( const MACAddress_t * pxMACAddress,
                               const uint32_t ulIPAddress )
{
    __CPROVER_assert( pxMACAddress != NULL, "pxMACAddress cannot be NULL" );
}

void vNDRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                           const IPv6_Address_t * pxIPAddress,
                           NetworkEndPoint_t * pxEndPoint )
{
    __CPROVER_assert( pxMACAddress != NULL, "pxMACAddress cannot be NULL" );
    __CPROVER_assert( pxIPAddress != NULL, "pxIPAddress cannot be NULL" );
    __CPROVER_assert( pxEndPoint != NULL, "pxEndPoint cannot be NULL" );
}

NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv4( uint32_t ulIPAddress )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    if( nondet_bool() )
    {
        pxEndpoint = pxNetworkEndPoints;
    }

    return pxEndpoint;
}

eFrameProcessingResult_t ProcessICMPPacket( const NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxEndPoint cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ), "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

eFrameProcessingResult_t prvProcessICMPMessage_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxEndPoint cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ), "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return 0;
}

BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                      uint16_t usPort )
{
    return 0;
}

void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

#if ( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )

uint16_t usGenerateChecksum( uint16_t usSum,
                             const uint8_t * pucNextData,
                             size_t uxByteCount )
{
    uint16_t usReturn;

    __CPROVER_assert( pucNextData != NULL, "Next data cannot be NULL" );

    return usReturn;
}

uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                     size_t uxBufferLength,
                                     BaseType_t xOutgoingPacket )
{
    uint16_t usReturn;

    __CPROVER_assert( pucEthernetBuffer != NULL, "Ethernet buffer cannot be NULL" );

    return usReturn;
}
#endif

void ProcessReceivedUDPPacket_harness()
{
    NetworkBufferDescriptor_t * const pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    uint8_t * pucEthernetBuffer = ( uint8_t * ) safeMalloc( ipTOTAL_ETHERNET_FRAME_SIZE + ipIP_TYPE_OFFSET );
    EthernetHeader_t * pxHeader;
    NetworkEndPoint_t xEndPoint;

    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = &( pucEthernetBuffer[ ipIP_TYPE_OFFSET ] );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->pxEndPoint = &xEndPoint;

    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( IPPacket_t ) + ipUDP_PACKET_TYPE_OFFSET && pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE );

    pxNetworkEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );
    __CPROVER_assume( pxNetworkEndPoints->pxNext == NULL );

    pxHeader = ( ( const EthernetHeader_t * ) pxNetworkBuffer->pucEthernetBuffer );
    __CPROVER_assume( pxHeader->usFrameType != ipIPv6_FRAME_TYPE );

    IPPacket_t * const pxIPPacket = ( IPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;
    UDPPacket_t * const pxUDPPacket = ( UDPPacket_t * ) ( pxIPPacket->xIPHeader.ucVersionHeaderLength + ( uint8_t * ) pxIPPacket );

    __CPROVER_assume( pxUDPPacket->xUDPPacket.usSourcePort != 0 );
    __CPROVER_assume( pxUDPPacket->xUDPPacket.usDestinationPort != 0 );

    uint16_t usPort = pxUDPPacket->xUDPPacket.usDestinationPort;

    xProcessReceivedUDPPacket( pxNetworkBuffer, usPort );
}