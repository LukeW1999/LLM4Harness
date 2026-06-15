```c
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"
/* CBMC includes. */
#include "cbmc.h"

/* This proof assumes that pxUDPSocketLookup is implemented correctly. */
/* This proof was done before. Hence we assume it to be correct here. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

/* This proof was done before. Hence we assume it to be correct here. */
BaseType_t xCheckRequiresARPResolution( const NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return nondet_BaseType_t();
}

/* Stub for pxUDPSocketLookup */
FreeRTOS_Socket_t * pxUDPSocketLookup( UBaseType_t uxLocalPort )
{
    /* Return either NULL or a valid socket nondeterministically */
    FreeRTOS_Socket_t * pxSocket = NULL;

    if( nondet_bool() )
    {
        pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );
        if( pxSocket != NULL )
        {
            /* Initialize socket fields nondeterministically */
            pxSocket->u.xUDP.pxHandleReceive = nondet_bool() ? NULL : ( void * ) 1;
            pxSocket->xEventGroup = nondet_bool() ? NULL : ( void * ) 1;
            pxSocket->u.xUDP.uxMaxPackets = nondet_UBaseType_t();
            pxSocket->u.xUDP.uxCurrentCount = nondet_UBaseType_t();
            /* Ensure count constraints */
            __CPROVER_assume( pxSocket->u.xUDP.uxMaxPackets <= 100 );
            __CPROVER_assume( pxSocket->u.xUDP.uxCurrentCount <= pxSocket->u.xUDP.uxMaxPackets );
            pxSocket->xSocketBits = nondet_EventBits_t();
            pxSocket->xReceiveBlockTime = nondet_TickType_t();
        }
    }

    return pxSocket;
}

/* Stub for vTaskSetTimeOutState */
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
}

/* Stub for xQueueGenericSend */
BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
    return nondet_BaseType_t();
}

/* Stub for vTaskSuspendAll */
void vTaskSuspendAll( void )
{
}

/* Stub for xTaskResumeAll */
BaseType_t xTaskResumeAll( void )
{
    return nondet_BaseType_t();
}

/* Stub for xEventGroupSetBits */
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet )
{
    return nondet_EventBits_t();
}

/* Stub for xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    return nondet_BaseType_t();
}

/* Stub for xSendEventToIPTask */
BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    return nondet_BaseType_t();
}

/* Stub for vReleaseNetworkBufferAndDescriptor */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
}

/* Stub for listCURRENT_LIST_LENGTH */
UBaseType_t listCURRENT_LIST_LENGTH_stub( List_t * pxList )
{
    return nondet_UBaseType_t();
}

/* Stub for uxQueueMessagesWaiting */
UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    UBaseType_t uxResult = nondet_UBaseType_t();
    return uxResult;
}

void ProcessReceivedUDPPacket_IPv6_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint16_t usPort;
    BaseType_t * pxIsWaitingForARPResolution;
    BaseType_t xResult;

    /* Allocate network buffer nondeterministically */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate buffer for the packet data */
    size_t xBufferLength;
    __CPROVER_assume( xBufferLength >= sizeof( UDPPacket_IPv6_t ) );
    __CPROVER_assume( xBufferLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xBufferLength;
    pxNetworkBuffer->usPort = nondet_uint16_t();
    pxNetworkBuffer->usBoundPort = nondet_uint16_t();
    pxNetworkBuffer->xIPAddress.ulIP_IPv4 = nondet_uint32_t();

    /* Set up the IPv6 addresses nondeterministically */
    size_t i;
    for( i = 0; i < 16; i++ )
    {
        pxNetworkBuffer->xIPAddress.xIP_IPv6.ucBytes[ i ] = nondet_uint8_t();
    }

    /* Set up the pxInterface and pxEndPoint */
    if( nondet_bool() )
    {
        pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    }
    else
    {
        pxNetworkBuffer->pxInterface = NULL;
    }

    if( nondet_bool() )
    {
        pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        if( pxNetworkBuffer->pxEndPoint != NULL )
        {
            /* Initialize endpoint fields nondeterministically */
            pxNetworkBuffer->pxEndPoint->bits.bIPv6 = nondet_uint8_t();
        }
    }
    else
    {
        pxNetworkBuffer->pxEndPoint = NULL;
    }

    /* Set up the UDP header within the buffer */
    if( xBufferLength >= sizeof( UDPPacket_IPv6_t ) )
    {
        UDPPacket_IPv6_t * pxUDPPacket = ( UDPPacket_IPv6_t * ) pxNetworkBuffer->pucEthernetBuffer;
        /* Nondeterministic packet contents already set by malloc */
    }

    /* Nondeterministic port */
    usPort = nondet_uint16_t();

    /* Allocate pxIsWaitingForARPResolution */
    pxIsWaitingForARPResolution = (