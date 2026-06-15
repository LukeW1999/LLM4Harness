/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"
/* CBMC includes. */
#include "cbmc.h"
/* This pointer is maintained by the IP-task. Defined in FreeRTOS_IP.c */
extern NetworkBufferDescriptor_t * pxARPWaitingNetworkBuffer;
NetworkEndPoint_t * pxNetworkEndPoint_Temp;

/* Stub FreeRTOS_FindEndPointOnNetMask_IPv6 as its not relevant to the
 * correctness of the proof */
eARPLookupResult_t FreeRTOS_FindEndPointOnNetMask_IPv6( const uint8_t * const pucIPCacheEntry,
                                                      NetworkEndPoint_t ** ppxEndPoint )
{
    __CPROVER_assert(0, "This function should not be called during the proof");
    return eARPCacheMiss;
}

void ARPProcessPacket_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = malloc(sizeof(NetworkBufferDescriptor_t));
    EthernetHeader_t * pxEthernetHeader = malloc(sizeof(EthernetHeader_t));
    ARPPacket_t * pxARPPacket = malloc(sizeof(ARPPacket_t));

    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxEthernetHeader != NULL);
    __CPROVER_assume(pxARPPacket != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)pxEthernetHeader;
    pxNetworkBuffer->xDataLength = sizeof(EthernetHeader_t) + sizeof(ARPPacket_t);

    pxEthernetHeader->usFrameType = ipARP_FRAME_TYPE;

    pxARPPacket = (ARPPacket_t *)(pxEthernetHeader + 1);
    pxARPPacket->usHardwareType = ipARP_HARDWARE_TYPE_ETHERNET;
    pxARPPacket->usProtocolType = ipARP_PROTOCOL_TYPE;
    pxARPPacket->ucHardwareAddressLength = ipMAC_ADDRESS_LENGTH_BYTES;
    pxARPPacket->ucProtocolAddressLength = ipIP_ADDRESS_LENGTH_BYTES;
    pxARPPacket->usOperation = nondet_uint16_t();

    pxNetworkEndPoint_Temp = malloc(sizeof(NetworkEndPoint_t));
    __CPROVER_assume(pxNetworkEndPoint_Temp != NULL);

    pxNetworkEndPoint_Temp->ipv4_settings.ulIPAddress = nondet_uint32_t();
    pxNetworkEndPoint_Temp->ipv4_settings.ulNetMask = nondet_uint32_t();
    pxNetworkEndPoint_Temp->ipv4_settings.ulGatewayAddress = nondet_uint32_t();
    pxNetworkEndPoint_Temp->ipv4_settings.ulDNSServerAddress = nondet_uint32_t();

    pxARPWaitingNetworkBuffer = pxNetworkBuffer;

    ARPProcessPacket(pxNetworkBuffer);

    /* Postconditions */
    assert(pxNetworkBuffer->eReleaseBuffer == pdFALSE || pxNetworkBuffer->eReleaseBuffer == pdTRUE);
    assert(pxNetworkBuffer->pxEndPoint == NULL || pxNetworkBuffer->pxEndPoint == pxNetworkEndPoint_Temp);
}