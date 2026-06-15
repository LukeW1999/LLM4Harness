/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"

/* CBMC includes. */
#include "cbmc.h"

/* The function prepareReplyDNSMessage is defined in FreeRTOS_DNS.c.
 * We declare it here as it may not be exposed in a header. */
void prepareReplyDNSMessage( NetworkBufferDescriptor_t * pxNetworkBuffer,
                             BaseType_t lNetLength );

void harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    BaseType_t lNetLength;
    size_t uxBufferLength;

    /* Allocate the network buffer descriptor. */
    pxNetworkBuffer = malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* The buffer must be large enough to hold at least a UDP packet header
     * plus a DNS message. Use a nondeterministic length bounded to a
     * reasonable size so CBMC can explore all paths without state-space
     * explosion. */
    __CPROVER_assume( uxBufferLength >= sizeof( UDPPacket_t ) + sizeof( DNSMessage_t ) );
    __CPROVER_assume( uxBufferLength <= sizeof( UDPPacket_t ) + sizeof( DNSMessage_t ) + 256U );

    pxNetworkBuffer->pucEthernetBuffer = malloc( uxBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = uxBufferLength;

    /* lNetLength represents the length of the DNS payload; it must be
     * non-negative and fit within the buffer. */
    __CPROVER_assume( lNetLength >= 0 );
    __CPROVER_assume( ( size_t ) lNetLength <= uxBufferLength );

    prepareReplyDNSMessage( pxNetworkBuffer, lNetLength );
}