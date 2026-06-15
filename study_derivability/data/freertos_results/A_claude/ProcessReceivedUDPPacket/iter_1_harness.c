/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
/* CBMC includes. */
#include "cbmc.h"

/* This proof assumes that pxUDPSocketLookup is implemented correctly. */
/* This proof was done before. Hence we assume it to be correct here. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

FreeRTOS_Socket_t * pxUDPSocketLookup( UBaseType_t uxLocalPort )
{
    return nondet_bool() ? ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) ) : NULL;
}

BaseType_t xCheckRequiresARPResolution( NetworkBufferDescriptor_t * pxNetworkBuffer )
{
    return nondet_BaseType_t();
}

BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    *pulNumber = nondet_uint32_t();
    return nondet_BaseType_t();
}

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    return nondet_uint32_t();
}

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    return nondet_BaseType_t();
}

void harness()
{
}

void ProcessReceivedUDPPacket_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    uint16_t usPort;
    BaseType_t * pxIsWaitingForARPResolution;
    BaseType_t xResult;

    /* Allocate network buffer */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate and constrain the ethernet buffer */
    size_t xBufferSize = sizeof( UDPPacket_t ) + ipBUFFER_PADDING + 1;
    uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = pucEthernetBuffer + ipBUFFER_PADDING;
    pxNetworkBuffer->xDataLength = nondet_size_t();
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( pxNetworkBuffer->xDataLength < xBufferSize );

    /* Nondeterministic port */
    usPort = nondet_uint16_t();

    /* Allocate pxIsWaitingForARPResolution */
    pxIsWaitingForARPResolution = ( BaseType_t * ) malloc( sizeof( BaseType_t ) );
    __CPROVER_assume( pxIsWaitingForARPResolution != NULL );

    /* Initialize network buffer fields */
    pxNetworkBuffer->usPort = nondet_uint16_t();
    pxNetworkBuffer->usBoundPort = nondet_uint16_t();
    pxNetworkBuffer->ulIPAddress = nondet_uint32_t();
    pxNetworkBuffer->xInterface = NULL;
    pxNetworkBuffer->pxNextBuffer = NULL;

    /* Initialize the Ethernet buffer contents nondeterministically */
    UDPPacket_t * pxUDPPacket = ( UDPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Call the function under test */
    xResult = ProcessReceivedUDPPacket( pxNetworkBuffer,
                                        usPort,
                                        pxIsWaitingForARPResolution );

    /* Assert postconditions */
    /* The result should be either pdPASS or pdFAIL */
    assert( xResult == pdPASS || xResult == pdFAIL );

    /* If we are not waiting for ARP resolution, the buffer should have been
     * properly handled */
    if( *pxIsWaitingForARPResolution == pdFALSE )
    {
        /* Result can be either pass or fail depending on socket lookup */
        assert( xResult == pdPASS || xResult == pdFAIL );
    }
    else
    {
        /* Waiting for ARP resolution means we couldn't process the packet yet */
        assert( *pxIsWaitingForARPResolution == pdTRUE );
    }
}