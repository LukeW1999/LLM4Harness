/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_NBNS.h"

/* CBMC includes. */
#include "cbmc.h"

void NBNSHandlePacket_harness()
{
    NetworkBufferDescriptor_t xNetworkBuffer;

    xNetworkBuffer.pucEthernetBuffer = malloc( sizeof( UDPPacket_t ) + sizeof( NBNSMessage_t ) );
    __CPROVER_assume( xNetworkBuffer.pucEthernetBuffer != NULL );

    NBNSHandlePacket( &xNetworkBuffer );
}