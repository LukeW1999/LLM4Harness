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
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
/* Include the stubs for APIs. */
#include "FreeRTOS_IP_stubs.h"
#include "FreeRTOS_ARP_stubs.h"
#include "FreeRTOS_UDP_IP_stubs.h"
#include "FreeRTOS_IP_Private_stubs.h"

void vProcessGeneratedUDPPacket_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    UDPPacket_t *pxUDPPacket;
    size_t uxPayloadLength;

    /* Allocate non-deterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    pxUDPPacket = (UDPPacket_t *)malloc(sizeof(UDPPacket_t));
    uxPayloadLength = nondet_size_t();

    /* Constrain inputs */
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxUDPPacket != NULL);
    __CPROVER_assume(uxPayloadLength <= ipconfigNETWORK_MTU - sizeof(UDPPacket_t) - ipIP_PACKET_SIZE);

    /* Set up the network buffer */
    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(ipconfigNETWORK_MTU);
    pxNetworkBuffer->xDataLength = sizeof(UDPPacket_t) + uxPayloadLength;
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipconfigNETWORK_MTU);

    /* Set up the UDP packet */
    pxNetworkBuffer->pucEthernetBuffer += ipIP_PACKET_SIZE;
    memcpy(pxNetworkBuffer->pucEthernetBuffer, pxUDPPacket, sizeof(UDPPacket_t));

    /* Call the function under test */
    vProcessGeneratedUDPPacket(pxNetworkBuffer);

    /* Assert postconditions */
    assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
    assert(pxNetworkBuffer->xDataLength >= sizeof(UDPPacket_t));

    /* Clean up */
    free(pxNetworkBuffer->pucEthernetBuffer - ipIP_PACKET_SIZE);
    free(pxNetworkBuffer);
    free(pxUDPPacket);
}