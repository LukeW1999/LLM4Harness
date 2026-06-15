#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc_proof/nondet.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DNS_NetworkContext.h"

void DNS_TreatNBNS_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    UDPPacket_t * pxUDPPacket;
    uint8_t ucPayload[256];
    size_t uxPayloadLength;

    /* Allocate and initialize pxNetworkBuffer */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);
    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(sizeof(UDPPacket_t) + sizeof(ucPayload));
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    pxUDPPacket = (UDPPacket_t *)pxNetworkBuffer->pucEthernetBuffer;
    pxNetworkBuffer->pucEthernetBuffer += ipUDP_PAYLOAD_OFFSET_IPv4;
    pxNetworkBuffer->pucEthernetBuffer -= ipIP_PAYLOAD_OFFSET_IPv4;
    pxNetworkBuffer->xDataLength = sizeof(UDPPacket_t) - ipIP_PAYLOAD_OFFSET_IPv4 + sizeof(ucPayload);

    /* Initialize payload */
    pxNetworkBuffer->pucEthernetBuffer += ipIP_PAYLOAD_OFFSET_IPv4;
    memcpy(pxNetworkBuffer->pucEthernetBuffer, ucPayload, sizeof(ucPayload));
    pxNetworkBuffer->pucEthernetBuffer -= ipIP_PAYLOAD_OFFSET_IPv4;

    /* Set up UDP packet header */
    pxUDPPacket->xIPHeader.usSourcePort = nondet_uint16_t();
    pxUDPPacket->xIPHeader.usDestinationPort = nondet_uint16_t();
    pxUDPPacket->xIPHeader.usLength = nondet_uint16_t();
    pxUDPPacket->xIPHeader.ucTimeToLive = nondet_uint8_t();
    pxUDPPacket->xIPHeader.ucVersionHeaderLength = nondet_uint8_t();
    pxUDPPacket->xIPHeader.ulSourceIPAddress = nondet_uint32_t();
    pxUDPPacket->xIPHeader.ulDestinationIPAddress = nondet_uint32_t();

    /* Set up NBNS payload */
    uxPayloadLength = nondet_size_t();
    __CPROVER_assume(uxPayloadLength <= sizeof(ucPayload));
    for (size_t i = 0; i < uxPayloadLength; i++) {
        ucPayload[i] = nondet_uint8_t();
    }

    /* Call the function under test */
    DNS_TreatNBNS(pxNetworkBuffer);

    /* Assert postconditions */
    assert(pxNetworkBuffer->xDataLength <= sizeof(UDPPacket_t) - ipIP_PAYLOAD_OFFSET_IPv4 + sizeof(ucPayload));
    assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
}