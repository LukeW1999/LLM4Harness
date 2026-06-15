#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc/nondet.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Timers.h"
#include "FreeRTOS_ARP.h"
#include "NetworkInterface.h"
#include "IPTraceMacroDefaults.h"

void ProcessICMPPacket_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    size_t uxBufferLength;

    // Allocate nondeterministic inputs
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    if (pxNetworkBuffer == NULL)
    {
        return; // Skip the test if malloc fails
    }

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(nondet_uintmax());
    uxBufferLength = nondet_uintmax();

    // Constrain inputs
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    __CPROVER_assume(uxBufferLength > sizeof(ethARPHeader_t) + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_ICMP_HEADER);
    __CPROVER_assume(uxBufferLength <= ipconfigNETWORK_MTU);

    // Set up the Ethernet header
    ethARPHeader_t *pxARPHeader = (ethARPHeader_t *)pxNetworkBuffer->pucEthernetBuffer;
    pxARPHeader->usFrameType = ipIPv4_FRAME_TYPE;

    // Set up the IP header
    IPPacket_t *pxIPPacket = (IPPacket_t *)(pxNetworkBuffer->pucEthernetBuffer + sizeof(ethARPHeader_t));
    pxIPPacket->xIPHeader.ucVersionHeaderLength = (4 << 4) | (ipSIZE_OF_IPv4_HEADER / 4);
    pxIPPacket->xIPHeader.usLength = FreeRTOS_htons((uint16_t)uxBufferLength - sizeof(ethARPHeader_t));
    pxIPPacket->xIPHeader.ulSourceIPAddress = nondet_uint32();
    pxIPPacket->xIPHeader.ulDestinationIPAddress = nondet_uint32();

    // Set up the ICMP header
    ICMPHeader_t *pxICMPHeader = (ICMPHeader_t *)(pxNetworkBuffer->pucEthernetBuffer + sizeof(ethARPHeader_t) + ipSIZE_OF_IPv4_HEADER);
    pxICMPHeader->ucTypeOfMessage = ipICMP_ECHO_REQUEST;
    pxICMPHeader->ucCode = 0;
    pxICMPHeader->usChecksum = 0; // Will be calculated by the function

    // Call the function under test
    ProcessICMPPacket(pxNetworkBuffer);

    // Assert postconditions
    assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
    assert(pxNetworkBuffer->xDataLength == uxBufferLength - sizeof(ethARPHeader_t));

    // Clean up
    free(pxNetworkBuffer->pucEthernetBuffer);
    free(pxNetworkBuffer);
}