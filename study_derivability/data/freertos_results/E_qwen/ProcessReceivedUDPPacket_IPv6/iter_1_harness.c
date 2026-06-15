/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

NetworkEndPoint_t xEndpoint;

eFrameProcessingResult_t __CPROVER_file_local_FreeRTOS_IP_c_prvProcessIPPacket( const IPPacket_t * pxIPPacket,
                                                                                NetworkBufferDescriptor_t * const pxNetworkBuffer );

/* Check if input is a valid extension header ID. */
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

/* Abstraction of xGetExtensionOrder. To ensure the result of prepared extension headers is same. */
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

/* proof is done separately */
eFrameProcessingResult_t ProcessICMPPacket( const NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxEndPoint cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ), "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

/* proof is done separately */
eFrameProcessingResult_t prvProcessICMPMessage_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxEndPoint cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ), "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

/* proof is done separately */
BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer )
{
}

/* proof is done separately */
BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                      uint16_t usPort )
{
}

/* This proof was done before. Hence we assume it to be correct here. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

eFrameProcessingResult_t publicProcessIPPacket( IPPacket_t * const pxIPPacket,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer );

#if ( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )

/* The checksum generation is stubbed out since the actual checksum
 * does not matter. The stub will return an indeterminate value each time. */
    uint16_t usGenerateChecksum( uint16_t usSum,
                                 const uint8_t * pucNextData,
                                 size_t uxByteCount )
    {
        uint16_t usReturn;

        __CPROVER_assert( pucNextData != NULL, "Next data cannot be NULL" );

        /* Return an indeterminate value. */
        return usReturn;
    }

/* The checksum generation is stubbed out since the actual checksum
 * does not matter. The stub will return an indeterminate value each time. */
    uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                         size_t uxBufferLength,
                                         BaseType_t xOutgoingPacket )
    {
        uint16_t usReturn;

        __CPROVER_assert( pucEthernetBuffer != NULL, "Ethernet buffer cannot be NULL" );

        /* Return an indeterminate value. */
        return usReturn;
    }
#endif /* if ( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 ) */

eFrameProcessingResult_t ProcessReceivedUDPPacket_IPv6( NetworkBufferDescriptor_t * pxNetworkBuffer );

void ProcessReceivedUDPPacket_IPv6_harness()
{
    NetworkBufferDescriptor_t * const pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    uint8_t * pucEthernetBuffer = ( uint8_t * ) safeMalloc( ipTOTAL_ETHERNET_FRAME_SIZE + ipIP_TYPE_OFFSET );
    EthernetHeader_t * pxHeader;
    NetworkEndPoint_t xEndPoint;

    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Points to ethernet buffer offset by ipIP_TYPE_OFFSET, this make sure the buffer allocation is similar
     * to the pxGetNetworkBufferWithDescriptor */
    pxNetworkBuffer->pucEthernetBuffer = &( pucEthernetBuffer[ ipIP_TYPE_OFFSET ] );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* prvProcessIPPacket is guaranteed to receive a network buffer that has a valid
     * endpoint, hence no NULL checks are needed to be performed inside prvProcessIPPacket.
     * See the check:
     *
     *  if( ( pxNetworkBuffer->pxInterface == NULL ) || ( pxNetworkBuffer->pxEndPoint == NULL ) )
     *  {
     *      break;
     *  }
     *
     * inside the prvProcessEthernetPacket before which prvProcessIPPacket is called.
     */
    pxNetworkBuffer->pxEndPoint = &xEndPoint;

    /* Minimum length of the pxNetworkBuffer->xDataLength is at least the size of the IPPacket_t. */
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( IPPacket_t ) && pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE );

    pxNetworkEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );
    __CPROVER_assume( pxNetworkEndPoints->pxNext == NULL );

    /* In this test case, we only focus on IPv6. */
    pxHeader = ( ( const EthernetHeader_t * ) pxNetworkBuffer->pucEthernetBuffer );
    __CPROVER_assume( pxHeader->usFrameType == ipIPv6_FRAME_TYPE );

    IPPacket_IPv6_t * const pxIPv6Packet = ( IPPacket_IPv6_t * ) pxNetworkBuffer->pucEthernetBuffer;
    UDPHeader_t * pxUDPHeader = ( UDPHeader_t * )( pxIPv6Packet->xPayload );

    /* Ensure the packet is a UDP packet */
    __CPROVER_assume( pxIPv6Packet->ucNextHeader == ipPROTOCOL_UDP );
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= ( ipIPV6_HEADER_SIZE + ipUDP_HEADER_SIZE ) );

    /* Initialize UDP Header fields */
    pxUDPHeader->usSourcePort = nondet_uint16_t();
    pxUDPHeader->usDestinationPort = nondet_uint16_t();
    pxUDPHeader->usLength = ( uint16_t )( pxNetworkBuffer->xDataLength - ipIPV6_HEADER_SIZE );
    pxUDPHeader->usChecksum = 0; /* Checksum is not verified in this harness */

    ProcessReceivedUDPPacket_IPv6( pxNetworkBuffer );
}