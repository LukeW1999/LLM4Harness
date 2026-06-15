/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

NetworkEndPoint_t xEndpoint;

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

NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    if( nondet_bool() )
    {
        pxEndpoint = pxNetworkEndPoints;
    }

    return pxEndpoint;
}

BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer )
{
}

BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                      uint16_t usPort )
{
}

void prvAllowIPPacketIPv6_harness()
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

    IPPacket_IPv6_t * const pxIPPacket = ( IPPacket_IPv6_t * ) pxNetworkBuffer->pucEthernetBuffer;
    prvAllowIPPacketIPv6( pxIPPacket, pxNetworkBuffer );
}