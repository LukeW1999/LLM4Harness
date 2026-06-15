#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

void SendData_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    NetworkInterface_t xInterfaces[2];
    size_t uxDataLength;
    BaseType_t xResult;

    // Allocate nondeterministic inputs
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(sizeof(uint8_t) * ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    uxDataLength = nondet_size_t();
    __CPROVER_assume(uxDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    xInterfaces[0].pvArgument = NULL;
    xInterfaces[0].pfInitialise = NULL;
    xInterfaces[0].pfInput = NULL;
    xInterfaces[0].pfOutput = NULL;
    xInterfaces[0].pfAddNetworkAddress = NULL;
    xInterfaces[0].pfSetMACAddress = NULL;
    xInterfaces[0].pfGetMACAddress = NULL;
    xInterfaces[0].pfInit = NULL;

    xInterfaces[1].pvArgument = NULL;
    xInterfaces[1].pfInitialise = NULL;
    xInterfaces[1].pfInput = NULL;
    xInterfaces[1].pfOutput = NULL;
    xInterfaces[1].pfAddNetworkAddress = NULL;
    xInterfaces[1].pfSetMACAddress = NULL;
    xInterfaces[1].pfGetMACAddress = NULL;
    xInterfaces[1].pfInit = NULL;

    // Set global interface pointer
    xNetworkInterfaceArray = xInterfaces;
    uxNetworkInterfaceNumber = 2;

    // Call the function under test
    xResult = SendData(pxNetworkBuffer, uxDataLength);

    // Assert postconditions
    if (xResult == pdPASS)
    {
        assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
        assert(pxNetworkBuffer->xDataLength == uxDataLength);
    }
    else
    {
        assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
    }

    // Free allocated memory
    free(pxNetworkBuffer->pucEthernetBuffer);
    free(pxNetworkBuffer);
}