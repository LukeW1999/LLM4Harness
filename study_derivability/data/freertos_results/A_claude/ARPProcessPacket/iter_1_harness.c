```c
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"
/* CBMC includes. */
#include "cbmc.h"

/* This pointer is maintained by the IP-task. Defined in FreeRTOS_IP.c */
extern NetworkBufferDescriptor_t * pxARPWaitingNetworkBuffer;

NetworkEndPoint_t * pxNetworkEndPoint_Temp;

/* Stub FreeRTOS_FindEndPointOnNetMask_IPv6 as its not relevant to the
 * correctness of the proof */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask_IPv6( const IPv6_Address_t * pxIPv6Address )
{
    return NULL;
}

/* Stub FreeRTOS_FindEndPointOnIP_IPv4 */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv4( uint32_t ulIPAddress,
                                                     uint32_t ulWhere )
{
    return pxNetworkEndPoint_Temp;
}

/* Stub FreeRTOS_FindEndPointOnNetMask */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress,
                                                     uint32_t ulWhere )
{
    return pxNetworkEndPoint_Temp;
}

/* Stub FreeRTOS_FindEndPointOnMAC */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnMAC( const MACAddress_t * pxMACAddress,
                                                 const NetworkInterface_t * pxInterface )
{
    return pxNetworkEndPoint_Temp;
}

/* Stub xIsCallingFromIPTask */
BaseType_t xIsCallingFromIPTask( void )
{
    return nondet_BaseType();
}

/* Stub vTaskSuspendAll */
void vTaskSuspendAll( void )
{
}

/* Stub xTaskResumeAll */
BaseType_t xTaskResumeAll( void )
{
    return pdTRUE;
}

/* Stub xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                      TickType_t uxTimeout )
{
    return nondet_BaseType();
}

/* Stub vARPRefreshCacheEntry */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                             const uint32_t ulIPAddress,
                             NetworkEndPoint_t * pxEndPoint )
{
}

/* Stub xNetworkInterfaceOutput */
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                     BaseType_t xReleaseAfterSend )
{
    return nondet_BaseType();
}

/* Stub vReleaseNetworkBufferAndDescriptor */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
}

/* Stub pxGetNetworkBufferWithDescriptor */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;

    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    if( pxNetworkBuffer != NULL )
    {
        pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xRequestedSizeBytes );

        if( pxNetworkBuffer->pucEthernetBuffer == NULL )
        {
            free( pxNetworkBuffer );
            pxNetworkBuffer = NULL;
        }
        else
        {
            pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
        }
    }

    return pxNetworkBuffer;
}

/* Stub uxIPHeaderSizePacket */
size_t uxIPHeaderSizePacket( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return ipSIZE_OF_IPv4_HEADER;
}

/* Stub FreeRTOS_FindGateWay */
NetworkEndPoint_t * FreeRTOS_FindGateWay( BaseType_t xIPType )
{
    return pxNetworkEndPoint_Temp;
}

void ARPProcessPacket_harness( void )
{
    ARPPacket_t * pxARPFrame;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    NetworkEndPoint_t * pxEndPoint;
    NetworkInterface_t * pxInterface;
    size_t uxBufferLength;

    /* Allocate network buffer */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate ethernet buffer large enough for ARP packet */
    uxBufferLength = sizeof( ARPPacket_t ) + ipBUFFER_PADDING;
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = sizeof( ARPPacket_t );

    /* Allocate and set up network interface */
    pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    __CPROVER_assume( pxInterface != NULL );
    pxNetworkBuffer->pxInterface = pxInterface;

    /* Allocate and set up endpoint */
    pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxEndPoint != NULL );

    /* Set up endpoint with nondeterministic IP and MAC addresses */
    pxEndPoint->ipv4_settings.ulIPAddress = nondet_uint32_t();
    pxEndPoint->ipv4_settings.ulNetMask = nondet_uint32_t();
    pxEndPoint->ipv4_settings.ulGatewayAddress = nondet_uint32_t();
    pxEndPoint->ipv4_settings.ulBroadcastAddress = nondet_uint32_t();
    pxEndPoint->bits.bEndPointUp = nondet_uint32_t() & 0x1U;
    pxEndPoint->pxNetworkInterface = pxInterface;
    pxEndPoint->pxNext = NULL;

    pxNetworkBuffer->pxEndPoint = pxEndPoint;

    /* Set pxNetworkEndPoint_Temp for stubs */
    pxNetworkEndPoint_Temp = pxEndPoint;

    /* Get the ARP frame from the buffer */
    pxARPFrame = ( ARPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Set nondeterministic ARP operation - either request or reply */
    uint16_t usOperation = nondet_uint16_t();
    /* Constrain to valid ARP operations: request (1) or reply (2) */
    __CPROVER_assume( ( usOperation == ipARP_REQUEST ) ||
                      ( usOperation == ipARP_REPLY ) );

    pxARPFrame->xARPHeader.usOperation = FreeRTOS_htons( usOperation );

    /* Set nondeterministic sender/target IP and MAC addresses */
    pxARPFrame->xARPHeader.ulSenderProtocolAddress = nondet_uint