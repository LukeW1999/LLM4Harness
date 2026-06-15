/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
/* CBMC includes. */
#include "memory_assignments.c"

/****************************************************************
* Signature of function under test
****************************************************************/
BaseType_t RecvFrom_CopyPacket( NetworkBufferDescriptor_t *pxNetworkBuffer, Socket_t xSocket );

/****************************************************************
* Harness function
****************************************************************/
void RecvFrom_CopyPacket_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    Socket_t xSocket;
    BaseType_t xResult;

    /* Allocate non-deterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    xSocket = nondet_Socket_t();

    /* Assume valid network buffer */
    __CPROVER_assume(pxNetworkBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);
    __CPROVER_assume(pxNetworkBuffer->xDataLength >= 0);
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    /* Assume valid socket */
    __CPROVER_assume(xSocket != FREERTOS_INVALID_SOCKET);

    /* Call function under test */
    xResult = RecvFrom_CopyPacket(pxNetworkBuffer, xSocket);

    /* Assert postconditions */
    if (xResult == pdPASS)
    {
        assert(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);
        assert(pxNetworkBuffer->pucEthernetBuffer != NULL);
    }
    else
    {
        assert(pxNetworkBuffer->xDataLength == 0 || pxNetworkBuffer->pucEthernetBuffer == NULL);
    }

    /* Free allocated memory */
    free(pxNetworkBuffer);
}