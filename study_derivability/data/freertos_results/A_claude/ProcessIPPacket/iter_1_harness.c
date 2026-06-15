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

        default:
            xReturn = pdFALSE;
            break;
    }

    return xReturn;
}

void ProcessIPPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xBufferLength;

    /* Allocate a network buffer descriptor nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* The buffer length must be at least large enough to hold an IP packet. */
    __CPROVER_assume( xBufferLength >= sizeof( IPPacket_t ) );
    __CPROVER_assume( xBufferLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    /* Allocate the network buffer data. */
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xBufferLength;

    /* Set the endpoint for the network buffer. */
    pxNetworkBuffer->pxEndPoint = &xEndpoint;

    /* Initialize the endpoint nondeterministically. */
    /* The endpoint's IP address can be anything. */

    /* Get a pointer to the IP packet within the buffer. */
    IPPacket_t * pxIPPacket = ( IPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Set up nondeterministic IP header fields. */
    /* The IP version can be 4 or 6. */
    uint8_t ucVersionHeaderLength;
    __CPROVER_assume( ( ucVersionHeaderLength >> 4 ) == 4 || ( ucVersionHeaderLength >> 4 ) == 6 );
    pxIPPacket->xIPHeader.ucVersionHeaderLength = ucVersionHeaderLength;

    /* Call the function under test. */
    eFrameProcessingResult_t eResult = __CPROVER_file_local_FreeRTOS_IP_c_prvProcessIPPacket( pxIPPacket, pxNetworkBuffer );

    /* Assert postconditions:
     * The result must be one of the valid eFrameProcessingResult_t values. */
    assert( eResult == eReleaseBuffer ||
            eResult == eProcessBuffer ||
            eResult == eReturnEthernetFrame ||
            eResult == eFrameConsumed ||
            eResult == eWaitingARPResolution );
}