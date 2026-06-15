#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Parser.h"
#include "FreeRTOS_IP_Private.h"
#include "cbmc.h"

void NBNSHandlePacket_harness()
{
    NetworkBufferDescriptor_t xNetworkBuffer;
    xNetworkBuffer.pucEthernetBuffer = (uint8_t *)malloc(nondet_uint());
    xNetworkBuffer.xDataLength = nondet_uint();

    __CPROVER_assume(xNetworkBuffer.pucEthernetBuffer != NULL);
    __CPROVER_assume(xNetworkBuffer.xDataLength <= ipconfigNETWORK_MTU);

    NBNSHandlePacket(&xNetworkBuffer);

    // Postconditions
    assert(xNetworkBuffer.pucEthernetBuffer != NULL);
    assert(xNetworkBuffer.xDataLength <= ipconfigNETWORK_MTU);
}