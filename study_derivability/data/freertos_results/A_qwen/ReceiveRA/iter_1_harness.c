#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc/nondet.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"

void ReceiveRA_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    pxNetworkBuffer->xDataLength = nondet_uint16_t();
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    pxNetworkBuffer->pxInterface = (NetworkInterface_t *)malloc(sizeof(NetworkInterface_t));
    __CPROVER_assume(pxNetworkBuffer->pxInterface != NULL);

    pxNetworkBuffer->pxInterface->pvArgument = (void *)nondet_pointer();

    EthernetHeader_t *pxEthernetHeader;
    pxEthernetHeader = (EthernetHeader_t *)pxNetworkBuffer->pucEthernetBuffer;

    pxEthernetHeader->usFrameType = ipIPv6_FRAME_TYPE;

    IPHeader_IPv6_t *pxIPHeader;
    pxIPHeader = (IPHeader_IPv6_t *)(pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER);

    pxIPHeader->ucVersionTrafficClassFlowLabel[0] = 0x60; // IPv6 version 6

    ICMPHeader_IPv6_t *pxICMPHeader;
    pxICMPHeader = (ICMPHeader_IPv6_t *)(pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER);

    pxICMPHeader->ucTypeOfMessage = ipICMP_RA;

    ReceiveRA(pxNetworkBuffer);

    // Postconditions
    assert(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);
    assert(pxNetworkBuffer->pxInterface != NULL);
    assert(pxEthernetHeader->usFrameType == ipIPv6_FRAME_TYPE);
    assert(pxIPHeader->ucVersionTrafficClassFlowLabel[0] == 0x60);
    assert(pxICMPHeader->ucTypeOfMessage == ipICMP_RA);

    free(pxNetworkBuffer->pucEthernetBuffer);
    free(pxNetworkBuffer->pxInterface);
    free(pxNetworkBuffer);
}