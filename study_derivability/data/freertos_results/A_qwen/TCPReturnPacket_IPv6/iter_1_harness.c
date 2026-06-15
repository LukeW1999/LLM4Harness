#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "cbmc.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "NetworkInterface.h"
#include "FreeRTOS_NDP.h"
#include "FreeRTOS_DHCP.h"

void TCPReturnPacket_IPv6_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    IPHeader_IPv6_t xIPHeader;
    size_t uxDataLength;

    // Allocate non-deterministic inputs
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(sizeof(uint8_t) * ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    pxNetworkBuffer->xDataLength = nondet_size_t();
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    memcpy(&xIPHeader, pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER, sizeof(IPHeader_IPv6_t));

    uxDataLength = nondet_size_t();
    __CPROVER_assume(uxDataLength <= pxNetworkBuffer->xDataLength - ipSIZE_OF_IPv6_HEADER);

    // Call the function under test
    TCPReturnPacket_IPv6(pxNetworkBuffer, uxDataLength);

    // Assert postconditions
    assert(pxNetworkBuffer->xDataLength >= ipSIZE_OF_IPv6_HEADER);
    assert(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);
    assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
}