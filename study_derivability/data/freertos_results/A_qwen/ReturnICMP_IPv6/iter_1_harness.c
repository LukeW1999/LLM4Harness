#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <cbmc_proof/nondet.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ICMP.h"

void ReturnICMP_IPv6_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer = nondet_bool() ? malloc(sizeof(NetworkBufferDescriptor_t)) : NULL;
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipconfigNETWORK_MTU);

    pxNetworkBuffer->xIPAddress.ulIP_IPv4 = nondet_uint32_t();
    pxNetworkBuffer->usPort = nondet_uint16_t();
    pxNetworkBuffer->usBoundPort = nondet_uint16_t();
    pxNetworkBuffer->ulIPAddress.xIP_IPv6.ucBytes = nondet_array_opaque(16);
    pxNetworkBuffer->usBufferOffset = nondet_uint16_t();
    __CPROVER_assume(pxNetworkBuffer->usBufferOffset <= ipconfigETHERNET_MINIMUM_PACKET_BYTES);

    IPHeader_IPv6_t *pxIPPacket = (IPHeader_IPv6_t *)(pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER);
    pxIPPacket->ucVersionTrafficClassFlowLabel = 0x60; // Version 6
    pxIPPacket->usPayloadLength = nondet_uint16_t();
    pxIPPacket->ucNextHeader = ipPROTOCOL_ICMP_IPv6;
    pxIPPacket->ucHopLimit = nondet_uint8_t();
    pxIPPacket->xSourceAddress.ucBytes = nondet_array_opaque(16);
    pxIPPacket->xDestinationAddress.ucBytes = nondet_array_opaque(16);

    ICMPHeader_IPv6_t *pxICMPHeader = (ICMPHeader_IPv6_t *)(pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER);
    pxICMPHeader->ucTypeOfMessage = nondet_uint8_t();
    pxICMPHeader->ucCode = nondet_uint8_t();
    pxICMPHeader->usChecksum = nondet_uint16_t();

    uint16_t usReturnedLength = ReturnICMP_IPv6(pxNetworkBuffer);

    // Postconditions
    assert(usReturnedLength <= ipconfigNETWORK_MTU);
    if (usReturnedLength > 0) {
        assert(pxNetworkBuffer->xDataLength == usReturnedLength);
    }
}