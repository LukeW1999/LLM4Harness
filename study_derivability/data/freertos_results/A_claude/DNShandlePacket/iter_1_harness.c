/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"
/* CBMC includes. */
#include "cbmc.h"

/* Function DNS_ParseDNSReply is proven to be correct separately. */
uint32_t DNS_ParseDNSReply( uint8_t * pucUDPPayloadBuffer,
                            size_t uxBufferLength,
                            struct freertos_addrinfo ** ppxAddressInfo,
                            BaseType_t xExpected,
                            uint16_t usPort )
{
    uint32_t ret;
    return ret;
}

void DNShandlePacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t uxPayloadLength;

    /* Allocate a nondeterministic network buffer descriptor. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* The network buffer must not be NULL for the function to proceed meaningfully. */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Nondeterministic payload length - must be at least large enough to hold
     * the UDP header and DNS header structures. */
    __CPROVER_assume( uxPayloadLength >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( uxPayloadLength <= ipconfigNETWORK_MTU );

    /* Allocate the buffer data with the given payload length. */
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( uxPayloadLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    /* Set the buffer length. */
    pxNetworkBuffer->xDataLength = uxPayloadLength;

    /* Set nondeterministic interface and endpoint pointers. */
    pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    /* Call the function under test. */
    DNShandlePacket( pxNetworkBuffer );

    /* No specific postconditions to assert beyond the function completing
     * without memory errors or undefined behavior, which CBMC checks
     * automatically. */
}