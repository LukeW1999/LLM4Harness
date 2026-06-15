/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

void prepareReplyDNSMessage( NetworkBufferDescriptor_t * pxNetworkBuffer,
                             const char * pcName,
                             uint16_t usTransactionId );

void prepareReplyDNSMessage_harness()
{
    NetworkBufferDescriptor_t xNetworkBuffer;
    char * pcName;

    xNetworkBuffer.pucEthernetBuffer = malloc( sizeof( UDPPacket_t ) + sizeof( DNSMessage_t ) );
    __CPROVER_assume( xNetworkBuffer.pucEthernetBuffer != NULL );

    pcName = malloc( 256 ); // Assuming a maximum name length of 255 characters plus null terminator
    __CPROVER_assume( pcName != NULL );

    // Assuming usTransactionId is within a reasonable range
    uint16_t usTransactionId = nondet_uint16();

    prepareReplyDNSMessage( &xNetworkBuffer, pcName, usTransactionId );
}