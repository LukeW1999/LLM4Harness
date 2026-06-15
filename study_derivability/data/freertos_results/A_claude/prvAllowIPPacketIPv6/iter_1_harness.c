/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IPv6.h"
#include "FreeRTOS_IP_Private.h"
/* CBMC includes. */
#include "cbmc.h"

eFrameProcessingResult_t prvAllowIPPacketIPv6( const IPHeader_IPv6_t * const pxIPv6Header,
                                               const NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                               UBaseType_t uxHeaderLength );

/* Create an endpoint and return, real endpoint doesn't matter in this test. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    static NetworkEndPoint_t xEndpoint;
    NetworkEndPoint_t * pxEndpoint;

    if( nondet_bool() )
    {
        pxEndpoint = NULL;
    }
    else
    {
        pxEndpoint = &xEndpoint;
    }

    return pxEndpoint;
}

void prvAllowIPPacketIPv6_harness( void )
{
    IPHeader_IPv6_t * pxIPv6Header;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    UBaseType_t uxHeaderLength;
    eFrameProcessingResult_t eResult;

    /* Allocate nondeterministic inputs */
    pxIPv6Header = ( IPHeader_IPv6_t * ) malloc( sizeof( IPHeader_IPv6_t ) );
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* Assume valid pointers */
    __CPROVER_assume( pxIPv6Header != NULL );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate network buffer data */
    size_t xBufferLength;
    __CPROVER_assume( xBufferLength >= sizeof( EthernetHeader_t ) + sizeof( IPHeader_IPv6_t ) );
    __CPROVER_assume( xBufferLength <= ipconfigNETWORK_MTU + sizeof( EthernetHeader_t ) );

    uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( xBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer;
    pxNetworkBuffer->xDataLength = xBufferLength;

    /* Allocate and set up endpoint for the network buffer */
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    if( nondet_bool() )
    {
        pxNetworkBuffer->pxEndPoint = NULL;
    }
    else
    {
        __CPROVER_assume( pxEndPoint != NULL );
        pxNetworkBuffer->pxEndPoint = pxEndPoint;
    }

    /* Constrain header length to reasonable values */
    __CPROVER_assume( uxHeaderLength <= ipconfigNETWORK_MTU );

    /* Call the function under test */
    eResult = prvAllowIPPacketIPv6( pxIPv6Header, pxNetworkBuffer, uxHeaderLength );

    /* Assert postconditions: result must be a valid eFrameProcessingResult_t value */
    assert( eResult == eProcessBuffer ||
            eResult == eReleaseBuffer ||
            eResult == eReturnEthernetFrame ||
            eResult == eFrameConsumed ||
            eResult == eWaitingARPResolution );
}