#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "cbmc.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_WIN.h"

void TCPPrepareSend_harness()
{
    struct xTCP_WINDOW xWindow;
    uint8_t *pucEthernetBuffer;
    size_t uxDataLength;
    size_t uxOffset;
    size_t uxReturn;

    // Allocate nondeterministic inputs
    pucEthernetBuffer = (uint8_t *)malloc(nondet_uint());
    uxDataLength = nondet_uint();
    uxOffset = nondet_uint();

    // Constrain inputs
    __CPROVER_assume(pucEthernetBuffer != NULL);
    __CPROVER_assume(uxDataLength > 0);
    __CPROVER_assume(uxOffset >= 0);
    __CPROVER_assume(uxOffset + uxDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    // Initialize xWindow with nondeterministic values
    xWindow.rx = (struct xTCP_SEGMENT *)malloc(sizeof(struct xTCP_SEGMENT));
    xWindow.tx = (struct xTCP_SEGMENT *)malloc(sizeof(struct xTCP_SEGMENT));
    xWindow.ulOurSequenceNumber = nondet_ulong();
    xWindow.ulTheirSequenceNumber = nondet_ulong();
    xWindow.ulAckSent = nondet_ulong();
    xWindow.ulRxWinSize = nondet_ulong();
    xWindow.ulTxWinSize = nondet_ulong();
    xWindow.ulRxWinSizeMax = nondet_ulong();
    xWindow.ulTxWinSizeMax = nondet_ulong();
    xWindow.ulNextTxSeq = nondet_ulong();
    xWindow.ulNextRxSeq = nondet_ulong();
    xWindow.ulHighestSequenceNumber = nondet_ulong();
    xWindow.ulFINSequenceNumber = nondet_ulong();
    xWindow.ulFINACKSequenceNumber = nondet_ulong();
    xWindow.ucMyWinScaleFactor = nondet_uchar();
    xWindow.ucPeerWinScaleFactor = nondet_uchar();
    xWindow.bits = nondet_uint();

    // Call the function under test
    uxReturn = TCPPrepareSend(&xWindow, pucEthernetBuffer, uxDataLength, uxOffset);

    // Assert postconditions
    assert(uxReturn <= uxDataLength);
    assert(uxReturn <= ipTOTAL_ETHERNET_FRAME_SIZE - uxOffset);
    if (uxReturn > 0) {
        assert(xWindow.ulNextTxSeq == xWindow.ulOurSequenceNumber + uxReturn);
    }

    // Free allocated memory
    free(pucEthernetBuffer);
    free(xWindow.rx);
    free(xWindow.tx);
}