/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
/* CBMC includes. */
#include "cbmc.h"

void ProcessReceivedUDPPacket_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    UDPPacket_t *pxUDPPacket;
    IPHeader_t *pxIPHeader;
    uint8_t *pucEthernetBuffer;
    size_t uxBufferLength;

    /* Allocate non-deterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    pucEthernetBuffer = (uint8_t *)malloc(nondet_size_t());
    uxBufferLength = nondet_size_t();

    /* Constrain inputs */
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pucEthernetBuffer != NULL);
    __CPROVER_assume(uxBufferLength > sizeof(IPHeader_t) + sizeof(UDPPacket_t));
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer == pucEthernetBuffer);
    __CPROVER_assume(pxNetworkBuffer->xDataLength == uxBufferLength);

    pxIPHeader = (IPHeader_t *)pucEthernetBuffer;
    pxUDPPacket = (UDPPacket_t *)(pucEthernetBuffer + sizeof(IPHeader_t));

    /* Initialize non-deterministic fields */
    pxIPHeader->ucVersionHeaderLength = 0x45; /* IPv4, header length 5 */
    pxIPHeader->usLength = uxBufferLength;
    pxUDPPacket->usLength = uxBufferLength - sizeof(IPHeader_t);

    /* Assume pxUDPSocketLookup is correct */
    __CPROVER_assume(pxUDPSocketLookup(pxUDPPacket->usSourcePort, pxUDPPacket->usDestinationPort) != NULL);

    /* Call the function under test */
    ProcessReceivedUDPPacket(pxNetworkBuffer);

    /* Assert postconditions */
    assert(pxNetworkBuffer->eReleaseBuffer == eReleaseBuffer);
}