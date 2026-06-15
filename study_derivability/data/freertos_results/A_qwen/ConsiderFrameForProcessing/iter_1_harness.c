#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"

void ConsiderFrameForProcessing_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    IPStackEvent_t xRxEvent;
    bool xResult;

    // Allocate nondeterministic inputs
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    xRxEvent.eEventType = nondet_uint8();
    xRxEvent.pvData = (void *)nondet_pointer();

    // Constrain inputs
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(xRxEvent.eEventType == eNetworkRxEvent);

    // Initialize pxNetworkBuffer fields as needed
    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)nondet_pointer();
    pxNetworkBuffer->xDataLength = nondet_uint16();
    pxNetworkBuffer->ulIPAddress_x = nondet_uint32();
    pxNetworkBuffer->usPort = nondet_uint16();
    pxNetworkBuffer->usBoundPort = nondet_uint16();
    pxNetworkBuffer->pxEndPoint = (NetworkEndPoint_t *)nondet_pointer();

    // Constrain pxNetworkBuffer fields
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->xDataLength > 0 && pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    // Call the function under test
    xResult = ConsiderFrameForProcessing(pxNetworkBuffer, &xRxEvent);

    // Assert postconditions
    assert(pxNetworkBuffer != NULL);
    if (xResult)
    {
        assert(xRxEvent.eEventType == eNetworkRxEvent);
        assert(xRxEvent.pvData == pxNetworkBuffer);
    }
    else
    {
        // If the frame is not processed, the event type should remain unchanged
        assert(xRxEvent.eEventType == eNetworkRxEvent);
        // The data pointer may or may not be changed, but it should not be NULL if it was not NULL initially
        if (xRxEvent.pvData != NULL)
        {
            assert(xRxEvent.pvData == pxNetworkBuffer);
        }
    }

    // Free allocated memory
    free(pxNetworkBuffer);
}