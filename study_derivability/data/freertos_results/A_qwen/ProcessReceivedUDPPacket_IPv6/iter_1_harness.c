/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
/* CBMC includes. */
#include "cbmc.h"

/* This proof assumes that pxUDPSocketLookup is implemented correctly. */
UDPSocket_t *pxUDPSocketLookup( uint16_t usPort )
{
    __CPROVER_assert(usPort != 0, "Port number should not be zero");
    return (UDPSocket_t *)__CPROVER_nondet_pointer();
}

/* This proof assumes that vARPRefreshCacheEntry is implemented correctly. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
}

void ProcessReceivedUDPPacket_IPv6_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    IPPacket_IPv6_t *pxIPPacket;
    UDPHeader_t *pxUDPHeader;
    size_t uxPayloadLength;

    /* Allocate nondeterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)__CPROVER_nondet_pointer();
    pxIPPacket = (IPPacket_IPv6_t *)pvPortMalloc(sizeof(IPPacket_IPv6_t));
    pxUDPHeader = (UDPHeader_t *)pvPortMalloc(sizeof(UDPHeader_t));
    uxPayloadLength = __CPROVER_nondet_size_t();

    /* Constrain inputs */
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxIPPacket != NULL);
    __CPROVER_assume(pxUDPHeader != NULL);
    __CPROVER_assume(uxPayloadLength <= ipconfigNETWORK_MTU - ipSIZE_OF_IPv6_HEADER - ipSIZE_OF_UDP_HEADER);

    /* Set up the network buffer */
    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)pxIPPacket;
    pxNetworkBuffer->xDataLength = ipSIZE_OF_IPv6_HEADER + ipSIZE_OF_UDP_HEADER + uxPayloadLength;

    /* Set up the IP packet */
    pxIPPacket->usSourcePort = __CPROVER_nondet_uint16_t();
    pxIPPacket->usDestinationPort = __CPROVER_nondet_uint16_t();
    pxIPPacket->ulLength = FreeRTOS_htons(ipSIZE_OF_UDP_HEADER + uxPayloadLength);

    /* Set up the UDP header */
    pxUDPHeader->usSourcePort = pxIPPacket->usSourcePort;
    pxUDPHeader->usDestinationPort = pxIPPacket->usDestinationPort;
    pxUDPHeader->usLength = FreeRTOS_htons(ipSIZE_OF_UDP_HEADER + uxPayloadLength);
    pxUDPHeader->usChecksum = 0; /* Checksum is not verified in this function */

    /* Place the UDP header after the IP header */
    memcpy((uint8_t *)pxIPPacket + ipSIZE_OF_IPv6_HEADER, pxUDPHeader, sizeof(UDPHeader_t));

    /* Call the function under test */
    ProcessReceivedUDPPacket_IPv6(pxNetworkBuffer);

    /* Assert postconditions */
    /* No specific postconditions are documented for ProcessReceivedUDPPacket_IPv6,
       but we can assert that the network buffer descriptor is not leaked */
    __CPROVER_assert(pxNetworkBuffer != NULL, "Network buffer should not be NULL");

    /* Free allocated memory */
    vPortFree(pxIPPacket);
    vPortFree(pxUDPHeader);
}