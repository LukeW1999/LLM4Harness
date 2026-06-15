/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_ND.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
/* Include the stubs for APIs. */
#include "stubs.h"

void vProcessGeneratedUDPPacket_IPv6_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    UDPPacket_t *pxUDPPacket;
    IPv6_Address_t xSourceAddress;
    IPv6_Address_t xDestinationAddress;
    uint16_t usSourcePort;
    uint16_t usDestinationPort;
    uint16_t usLength;

    /* Allocate nondeterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    pxUDPPacket = (UDPPacket_t *)malloc(sizeof(UDPPacket_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxUDPPacket != NULL);

    /* Initialize the network buffer descriptor */
    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    pxNetworkBuffer->xDataLength = __CPROVER_nondet_uint16();
    __CPROVER_assume(pxNetworkBuffer->xDataLength >= sizeof(UDPPacket_t));

    /* Initialize the UDP packet */
    pxUDPPacket = (UDPPacket_t *)pxNetworkBuffer->pucEthernetBuffer;
    pxUDPPacket->xIPHeader.usLength = __CPROVER_nondet_uint16();
    pxUDPPacket->xIPHeader.xSourceAddress = xSourceAddress;
    pxUDPPacket->xIPHeader.xDestinationAddress = xDestinationAddress;
    pxUDPPacket->xUDPHeader.usSourcePort = usSourcePort;
    pxUDPPacket->xUDPHeader.usDestinationPort = usDestinationPort;
    pxUDPPacket->xUDPHeader.usLength = usLength;

    /* Constrain inputs */
    __CPROVER_assume(usLength >= sizeof(UDPPacket_t) - ipSIZE_OF_ETH_HEADER);
    __CPROVER_assume(usLength <= pxNetworkBuffer->xDataLength);
    __CPROVER_assume(usSourcePort > 0);
    __CPROVER_assume(usDestinationPort > 0);

    /* Call the function under test */
    vProcessGeneratedUDPPacket_IPv6(pxNetworkBuffer);

    /* Assert postconditions */
    assert(pxNetworkBuffer->xDataLength >= sizeof(UDPPacket_t) - ipSIZE_OF_ETH_HEADER);
    assert(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);
    assert(pxUDPPacket->xUDPHeader.usLength == usLength);
    assert(pxUDPPacket->xUDPHeader.usSourcePort == usSourcePort);
    assert(pxUDPPacket->xUDPHeader.usDestinationPort == usDestinationPort);

    /* Free allocated memory */
    free(pxNetworkBuffer->pucEthernetBuffer);
    free(pxNetworkBuffer);
}