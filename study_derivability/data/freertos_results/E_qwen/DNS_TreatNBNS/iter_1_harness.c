/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

void DNS_TreatNBNS( NetworkBufferDescriptor_t * pxNetworkBuffer );

void DNS_TreatNBNS_harness()
{
    NetworkBufferDescriptor_t xNetworkBuffer;

    xNetworkBuffer.pucEthernetBuffer = malloc( sizeof( UDPPacket_t ) + sizeof( NBNSRequest_t ) );
    __CPROVER_assume( xNetworkBuffer.pucEthernetBuffer != NULL );

    DNS_TreatNBNS( &xNetworkBuffer );
}