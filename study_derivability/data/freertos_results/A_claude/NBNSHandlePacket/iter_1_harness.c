/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Parser.h"
#include "FreeRTOS_IP_Private.h"
#include "cbmc.h"

NetworkBufferDescriptor_t xNetworkBuffer;

/* DNS_TreatNBNS is proved separately */
void DNS_TreatNBNS( uint8_t * pucPayload,
                    size_t uxBufferLength,
                    uint32_t ulIPAddress )
{
    /* Stub - proved separately */
}

void NBNSHandlePacket_harness( void )
{
    /* Allocate a nondeterministic network buffer */
    size_t uxBufferLength;

    /* The buffer must be large enough to hold at least the UDP packet header
     * plus some payload. Constrain to a reasonable range. */
    __CPROVER_assume( uxBufferLength >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU );

    xNetworkBuffer.xDataLength = uxBufferLength;

    /* Allocate nondeterministic buffer data */
    uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    xNetworkBuffer.pucEthernetBuffer = pucEthernetBuffer;

    /* Call the function under test */
    NBNSHandlePacket( &xNetworkBuffer );

    /* No specific postconditions to assert beyond no crash/undefined behavior */
}