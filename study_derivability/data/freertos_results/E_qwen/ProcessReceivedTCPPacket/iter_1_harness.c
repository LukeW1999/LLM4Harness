#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "cbmc.h"

NetworkEndPoint_t xEndpoint;

eFrameProcessingResult_t __CPROVER_file_local_FreeRTOS_IP_c_prvProcessIPPacket( const IPPacket_t * pxIPPacket,
                                                                                NetworkBufferDescriptor_t * const pxNetworkBuffer );

BaseType_t xIsExtensionHeader( uint8_t ucCurrentHeader )
{
    BaseType_t xReturn = pdFALSE;

    switch( ucCurrentHeader )
    {
        case ipIPv6_EXT_HEADER_HOP_BY_HOP:
        case ipIPv6_EXT_HEADER_DESTINATION_OPTIONS:
        case ipIPv6_EXT_HEADER_ROUTING_HEADER:
        case ipIPv6_EXT_HEADER_FRAGMENT_HEADER:
        case ipIPv6_EXT_HEADER_AUTHEN_HEADER:
        case ipIPv6_EXT_HEADER_SECURE_PAYLOAD:
        case ipIPv6_EXT_HEADER_MOBILITY_HEADER:
            xReturn = pdTRUE;
            break;
    }

    return xReturn;
}

BaseType_t xGetExtensionOrder( uint8_t ucProtocol,
                               uint8_t ucNextHeader )
{
    return xIsExtensionHeader( ucProtocol );
}

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
}

BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                      uint16_t usPort )
{
}

void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

eFrameProcessingResult_t publicProcessIPPacket( IPPacket_t * const pxIPPacket,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer );

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

void ProcessReceivedTCPPacket_harness()
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

    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( IPPacket_t ) + ipSIZE_OF_TCP_HEADER && pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE );

    pxNetworkEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );
    __CPROVER_assume( pxNetworkEndPoints->pxNext == NULL );

    pxHeader = ( ( const EthernetHeader_t * ) pxNetworkBuffer->pucEthernetBuffer );
    __CPROVER_assume( pxHeader->usFrameType != ipIPv6_FRAME_TYPE );

    IPPacket_t * const pxIPPacket = ( IPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;
    TCPHeader_t * const pxTCPHeader = ( TCPHeader_t * ) ( pxIPPacket->xIPHeader.ucVersionHeaderLength + ( uint8_t * ) pxIPPacket );

    __CPROVER_assume( pxTCPHeader->ucProtocol == ipPROTOCOL_TCP );

    __CPROVER_file_local_FreeRTOS_IP_c_prvProcessIPPacket( pxIPPacket, pxNetworkBuffer );
}