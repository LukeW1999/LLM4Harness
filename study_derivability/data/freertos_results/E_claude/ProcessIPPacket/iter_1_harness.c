```c
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv4_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/* Stub for usGenerateProtocolChecksum - checksum value doesn't matter for verification. */
uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                     size_t uxBufferLength,
                                     BaseType_t xOutgoingPacket )
{
    uint16_t usReturn;

    __CPROVER_assert( pucEthernetBuffer != NULL, "Ethernet buffer cannot be NULL" );

    return usReturn;
}

/* Stub for usGenerateChecksum. */
uint16_t usGenerateChecksum( uint16_t usSum,
                             const uint8_t * pucNextData,
                             size_t uxByteCount )
{
    uint16_t usReturn;

    return usReturn;
}

/* Stub for FreeRTOS_FindEndPointOnIP_IPv4. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv4( uint32_t ulIPAddress )
{
    NetworkEndPoint_t * pxReturn = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    return pxReturn;
}

/* Stub for FreeRTOS_FindEndPointOnMAC. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                const NetworkInterface_t * pxInterface )
{
    NetworkEndPoint_t * pxReturn = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    __CPROVER_assert( pxMACAddress != NULL, "MAC address shouldn't be NULL" );

    return pxReturn;
}

/* Stub for FreeRTOS_FindEndPointOnNetMask. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress )
{
    NetworkEndPoint_t * pxReturn = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    return pxReturn;
}

/* Stub for FreeRTOS_IsNetworkUp. */
BaseType_t FreeRTOS_IsNetworkUp( void )
{
    BaseType_t xReturn;

    return xReturn;
}

/* Stub for xProcessReceivedTCPPacket. */
BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t * pxDescriptor )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxDescriptor != NULL, "pxDescriptor cannot be NULL" );

    return xReturn;
}

/* Stub for xProcessReceivedUDPPacket. */
BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t * pxDescriptor,
                                      uint16_t usPort,
                                      BaseType_t * pxIsWaitingForARPResolution )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxDescriptor != NULL, "pxDescriptor cannot be NULL" );
    __CPROVER_assert( pxIsWaitingForARPResolution != NULL, "pxIsWaitingForARPResolution cannot be NULL" );

    /* Non-deterministically set the waiting for ARP resolution flag. */
    *pxIsWaitingForARPResolution = nondet_bool() ? pdTRUE : pdFALSE;

    return xReturn;
}

/* Stub for vARPRefreshCacheEntry. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress,
                            NetworkEndPoint_t * pxEndPoint )
{
}

/* Stub for eARPProcessPacket. */
eFrameProcessingResult_t eARPProcessPacket( const NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    eFrameProcessingResult_t eReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer cannot be NULL" );

    return eReturn;
}

/* Stub for vReturnEthernetFrame. */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer cannot be NULL" );
}

/* Stub for vReleaseNetworkBufferAndDescriptor. */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer cannot be NULL" );
}

/* Stub for xSendEventStructToIPTask. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxEvent != NULL, "pxEvent cannot be NULL" );

    return xReturn;
}

/* Stub for pvPortMalloc. */
void * pvPortMalloc( size_t xSize )
{
    return safeMalloc( xSize );
}

/* Stub for vPortFree. */
void vPortFree( void * pv )
{
}

/* Function under test declaration. */
eFrameProcessingResult_t ProcessIPPacket( const NetworkBufferDescriptor_t * const pxNetworkBuffer );

void ProcessIPPacket_harness()
{
    NetworkBufferDescriptor_t * const pxNetworkBuffer = safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    uint8_t * pucEthernetBuffer = ( uint8_t * ) safeMalloc( ipTOTAL_ETHERNET_FRAME_SIZE + ipIP_TYPE_OFFSET );

    /* Network buffer must be valid. */
    __CPROVER_assume( pxNetworkBuffer != NULL );
    /* Ethernet buffer must be valid. */
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Points to ethernet buffer offset by ipIP_TYPE_OFFSET, matching pxGetNetworkBufferWithDescriptor allocation. */
    pxNetworkBuffer->pucEthernetBuffer = &( pucEthernetBuffer[ ipIP_TYPE_OFFSET ] );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* xDataLength must be at least the size of IPPacket_t (IPv4). */
    __CPROVER_assume( ( pxNetworkBuffer->xDataLength >= sizeof( IPPacket_t ) ) &&
                      ( pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE ) );

    /* Set up network endpoints. */
    pxNetworkEndPoints = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );

    /* Initialize endpoint fields. */
    pxNetworkEndPoints->pxNetworkInterface = ( NetworkInterface_t * ) safeMalloc( sizeof( NetworkInterface_t ) );

    if( nondet_bool() )
    {
        pxNetworkEndPoints->pxNext = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );
        __CPROVER_assume( pxNetworkEndPoints->pxNext != NULL );
        pxNetwork