```c
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"

/* CBMC includes. */
#include "cbmc.h"

/* The function under test. */
BaseType_t ProcessReceivedUDPPacket_IPv6( NetworkBufferDescriptor_t * pxNetworkBuffer,
                                          uint16_t usPort,
                                          BaseType_t * pxIsWaitingForARPResolution );

/* Stub for xApplicationGetRandomNumber */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    BaseType_t xReturn;

    __CPROVER_assert( pulNumber != NULL, "pulNumber cannot be NULL" );

    *pulNumber = nondet_uint32_t();
    return xReturn;
}

/* Stub for FreeRTOS_FindEndPointOnIP_IPv6 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    NetworkEndPoint_t * pxEndpoint = NULL;

    __CPROVER_assert( pxIPAddress != NULL, "pxIPAddress cannot be NULL" );

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

/* Stub for vNDRefreshCacheEntry */
void vNDRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                           const IPv6_Address_t * pxIPAddress,
                           NetworkEndPoint_t * pxEndPoint )
{
    __CPROVER_assert( pxMACAddress != NULL, "pxMACAddress cannot be NULL" );
    __CPROVER_assert( pxIPAddress != NULL, "pxIPAddress cannot be NULL" );
    __CPROVER_assert( pxEndPoint != NULL, "pxEndPoint cannot be NULL" );
}

/* Stub for vARPRefreshCacheEntry */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

/* Stub for xCheckRequiresResolution */
BaseType_t xCheckRequiresResolution( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxNetworkBuffer != NULL, "pxNetworkBuffer cannot be NULL" );
    __CPROVER_assert( __CPROVER_r_ok( pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength ),
                      "Data in pxNetworkBuffer must be readable" );

    return xReturn;
}

/* Stub for vSocketWakeUpUser - wake up a user task waiting for data */
void vSocketWakeUpUser( FreeRTOS_Socket_t * pxSocket )
{
    __CPROVER_assert( pxSocket != NULL, "pxSocket cannot be NULL" );
}

/* Stub for xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxEvent != NULL, "pxEvent cannot be NULL" );

    return xReturn;
}

/* Stub for vTaskSetTimeOutState */
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
    __CPROVER_assert( pxTimeOut != NULL, "pxTimeOut cannot be NULL" );
}

/* Stub for xTaskCheckForTimeOut */
BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut,
                                 TickType_t * const pxTicksToWait )
{
    BaseType_t xReturn;

    __CPROVER_assert( pxTimeOut != NULL, "pxTimeOut cannot be NULL" );
    __CPROVER_assert( pxTicksToWait != NULL, "pxTicksToWait cannot be NULL" );

    return xReturn;
}

/* Stub for xQueueGenericSend */
BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                               const void * const pvItemToQueue,
                               TickType_t xTicksToWait,
                               const BaseType_t xCopyPosition )
{
    BaseType_t xReturn;

    return xReturn;
}

/* Stub for vPortEnterCritical */
void vPortEnterCritical( void )
{
}

/* Stub for vPortExitCritical */
void vPortExitCritical( void )
{
}

/* Stub for xTaskGetSchedulerState */
BaseType_t xTaskGetSchedulerState( void )
{
    BaseType_t xReturn;

    return xReturn;
}

/* Stub for pxUDPSocketLookup - find socket for a given port */
FreeRTOS_Socket_t * pxUDPSocketLookup( UBaseType_t uxLocalPort )
{
    FreeRTOS_Socket_t * pxSocket = NULL;

    if( nondet_bool() )
    {
        pxSocket = ( FreeRTOS_Socket_t * ) safeMalloc( sizeof( FreeRTOS_Socket_t ) );
    }

    return pxSocket;
}

/* Stub for xIPIsNetworkTaskReady */
BaseType_t xIPIsNetworkTaskReady( void )
{
    BaseType_t xReturn;

    return xReturn;
}

/* Stub for usGenerateChecksum */
uint16_t usGenerateChecksum( uint16_t usSum,
                             const uint8_t * pucNextData,
                             size_t uxByteCount )
{
    uint16_t usReturn;

    __CPROVER_assert( pucNextData != NULL, "Next data cannot be NULL" );

    return usReturn;
}

/* Stub for usGenerateProtocolChecksum */
uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer,
                                     size_t uxBufferLength,
                                     BaseType_t xOutgoingPacket )
{
    uint16_t usReturn;

    __CPROVER_assert( pucEthernetBuffer != NULL, "Ethernet buffer cannot be NULL" );

    return usReturn;
}

void ProcessReceivedUDPPacket_IPv6_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint16_t usPort;
    BaseType_t xIsWaitingForARPResolution;
    uint8_t * pucEthernetBuffer;
    NetworkEndPoint_t xEndPoint;
    size_t xDataLength;

    /* Allocate the network buffer descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) safeMalloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate ethernet buffer large enough to hold an IPv6 UDP packet.
     * Minimum size: Ethernet header + IPv6 header + UDP header + some payload */
    xDataLength = sizeof( UDPPacket_IPv