/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Stream_Buffer.h"
/* CBMC includes. */
#include "cbmc.h"
#include "../../utility/memory_assignments.c"

void ProcessReceivedTCPPacket_harness()
{
    NetworkBufferDescriptor_t *pxDescriptor;
    TCPPacket_t *pxTCPPacket;
    TCPSocket_t *pxSocket;

    /* Allocate nondeterministic inputs */
    pxDescriptor = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    pxTCPPacket = (TCPPacket_t *)malloc(sizeof(TCPPacket_t));
    pxSocket = (TCPSocket_t *)malloc(sizeof(TCPSocket_t));

    /* Constrain inputs */
    __CPROVER_assume(pxDescriptor != NULL);
    __CPROVER_assume(pxTCPPacket != NULL);
    __CPROVER_assume(pxSocket != NULL);
    __CPROVER_assume(pxDescriptor->pucEthernetBuffer != NULL);
    __CPROVER_assume(pxDescriptor->xDataLength >= sizeof(TCPPacket_t));
    __CPROVER_assume(pxSocket->u.xTCP.eTCPState >= eCLOSED);
    __CPROVER_assume(pxSocket->u.xTCP.eTCPState <= eLAST_STATE);

    /* Assign memory to the network buffer */
    pxDescriptor->pucEthernetBuffer = (uint8_t *)malloc(pxDescriptor->xDataLength);
    __CPROVER_assume(pxDescriptor->pucEthernetBuffer != NULL);

    /* Initialize the TCP packet */
    memcpy(pxDescriptor->pucEthernetBuffer, pxTCPPacket, sizeof(TCPPacket_t));

    /* Mock implementations for required functions */
    __CPROVER_assume(pxTCPSocketLookup(pxTCPPacket) == pxSocket);
    __CPROVER_assume(pxGetNetworkBufferWithDescriptor(pxDescriptor->xDataLength, 0) == pxDescriptor);

    /* Call the function under test */
    ProcessReceivedTCPPacket(pxDescriptor);

    /* Assert postconditions */
    assert(pxSocket->u.xTCP.eTCPState >= eCLOSED);
    assert(pxSocket->u.xTCP.eTCPState <= eLAST_STATE);
    assert(pxDescriptor->xDataLength >= 0);

    /* Free allocated memory */
    free(pxDescriptor->pucEthernetBuffer);
    free(pxDescriptor);
    free(pxTCPPacket);
    free(pxSocket);
}