#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc_proof/nondet.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS_NetworkContext.h"

void prepareReplyDNSMessage_harness()
{
    struct xNetworkBufferDescriptor *pxNetworkBuffer = malloc(sizeof(struct xNetworkBufferDescriptor));
    struct xDNSMessage *pxDNSMessage = malloc(sizeof(struct xDNSMessage));
    uint8_t ucPayload[512];
    size_t uxPayloadLength;

    if (pxNetworkBuffer == NULL || pxDNSMessage == NULL)
    {
        return;
    }

    pxNetworkBuffer->pucEthernetBuffer = ucPayload;
    pxNetworkBuffer->xDataLength = sizeof(ucPayload);

    pxDNSMessage->ucIdentifier = nondet_uint8_t();
    pxDNSMessage->ucFlags = nondet_uint8_t();
    pxDNSMessage->usQuestions = nondet_uint16_t();
    pxDNSMessage->usAnswers = nondet_uint16_t();
    pxDNSMessage->usAuthorityRRs = nondet_uint16_t();
    pxDNSMessage->usAdditionalRRs = nondet_uint16_t();

    uxPayloadLength = nondet_size_t();
    __CPROVER_assume(uxPayloadLength <= sizeof(ucPayload));

    prepareReplyDNSMessage(pxNetworkBuffer, pxDNSMessage, uxPayloadLength);

    // Postconditions
    assert(pxNetworkBuffer->xDataLength <= sizeof(ucPayload));
    assert(pxNetworkBuffer->xDataLength >= sizeof(struct xDNSMessageHeader));
    assert(pxDNSMessage->ucIdentifier == pxDNSMessage->ucIdentifier); // Identity check for identifier
    assert(pxDNSMessage->ucFlags & DNS_FLAG_RESPONSE); // Check response flag is set
}

int main()
{
    prepareReplyDNSMessage_harness();
    return 0;
}