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

/* Stub for vReturnEthernetFrame which is proven separately. */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    /* Nothing to do. */
}

/* Stub for xApplicationDNSQueryHook which is proven separately. */
BaseType_t xApplicationDNSQueryHook( const char * pcName )
{
    return nondet_BaseType();
}

/* Stub for FreeRTOS_SendUDPPacket which may be called internally. */
BaseType_t FreeRTOS_sendto( Socket_t xSocket,
                            const void * pvBuffer,
                            size_t uxTotalDataLength,
                            BaseType_t xFlags,
                            const struct freertos_sockaddr * pxDestinationAddress,
                            socklen_t xDestinationAddressLength )
{
    return nondet_BaseType();
}

void harness()
{
    size_t uxBufferLength;
    uint32_t ulIPAddress;

    /* uxBufferLength must be at least large enough to be a valid NBNS payload.
     * We bound it to avoid state space explosion. */
    __CPROVER_assume( uxBufferLength >= sizeof( NBNSRequest_t ) );
    __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU );

    uint8_t * pucPayload = malloc( uxBufferLength );
    __CPROVER_assume( pucPayload != NULL );

    /* ulIPAddress can be any value. */
    DNS_TreatNBNS( pucPayload, uxBufferLength, ulIPAddress );
}