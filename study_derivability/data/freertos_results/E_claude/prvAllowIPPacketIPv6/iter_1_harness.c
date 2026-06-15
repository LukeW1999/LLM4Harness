/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"

/* CBMC includes. */
#include "cbmc.h"

/* Declaration of the function under test. */
eFrameProcessingResult_t __CPROVER_file_local_FreeRTOS_IP_Utils_c_prvAllowIPPacketIPv6( const IPHeader_IPv6_t * const pxIPv6Header,
                                                                                         const NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                                                         size_t uxHeaderLength );

/* Stub for FreeRTOS_FindEndPointOnIPv6 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIPv6( const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    if( nondet_bool() )
    {
        pxEndpoint = pxNetworkEndPoints;
    }

    return pxEndpoint;
}

/* Stub for FreeRTOS_FindEndPointOnMAC */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    if( nondet_bool() )
    {
        pxEndpoint = pxNetworkEndPoints;
    }

    return pxEndpoint;
}

/* Stub for FreeRTOS_FindEndPointOnIP_IPv4 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv4( uint32_t ulIPAddress )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    if( nondet_bool() )
    {
        pxEndpoint = pxNetworkEndPoints;
    }

    return pxEndpoint;
}

/* Stub for usGenerateProtocolChecksum */
#if ( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
    uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                         size_t uxBufferLength,
                                         BaseType_t xOutgoingPacket )
    {
        uint16_t usReturn;

        __CPROVER_assert( pucEthernetBuffer != NULL, "Ethernet buffer cannot be NULL" );

        return usReturn;
    }

    uint16_t usGenerateChecksum( uint16_t usSum,
                                 const uint8_t * pucNextData,
                                 size_t uxByteCount )
    {
        uint16_t usReturn;

        __CPROVER_assert( pucNextData != NULL, "Next data cannot be NULL" );

        return usReturn;
    }
#endif /* if ( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 ) */

void prvAllowIPPacketIPv6_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint8_t * pucEthernetBuffer;
    size_t uxHeaderLength;
    NetworkEndPoint_t xEndPoint;
    NetworkInterface_t xInterface;

    /* Allocate network buffer descriptor */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate ethernet buffer with extra space for IP_TYPE_OFFSET */
    pucEthernetBuffer = ( uint8_t * ) safeMalloc( ipTOTAL_ETHERNET_FRAME_SIZE + ipIP_TYPE_OFFSET );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Points to ethernet buffer offset by ipIP_TYPE_OFFSET, similar to pxGetNetworkBufferWithDescriptor */
    pxNetworkBuffer->pucEthernetBuffer = &( pucEthernetBuffer[ ipIP_TYPE_OFFSET ] );

    /* Set a valid data length: at least large enough for IPv6 packet */
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( IPPacket_IPv6_t ) &&
                      pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE );

    /* Set up endpoint */
    pxNetworkBuffer->pxEndPoint = &xEndPoint;
    pxNetworkBuffer->pxInterface = &xInterface;

    /* Set up global endpoints list */
    pxNetworkEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );
    __CPROVER_assume( pxNetworkEndPoints->pxNext == NULL );

    /* uxHeaderLength can be any value - the function uses it for checksum validation */
    /* Constrain to reasonable bounds */
    __CPROVER_assume( uxHeaderLength <= ipTOTAL_ETHERNET_FRAME_SIZE );

    /* Get pointer to IPv6 header from the network buffer */
    const IPHeader_IPv6_t * pxIPv6Header = ( const IPHeader_IPv6_t * ) &( pxNetworkBuffer->pucEthernetBuffer[ ipSIZE_OF_ETH_HEADER ] );

    /* Call the function under test */
    __CPROVER_file_local_FreeRTOS_IP_Utils_c_prvAllowIPPacketIPv6( pxIPv6Header,
                                                                    pxNetworkBuffer,
                                                                    uxHeaderLength );
}