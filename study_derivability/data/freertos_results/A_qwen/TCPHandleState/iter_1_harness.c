#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_WIN.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_TCP_Transmission.h"
#include "FreeRTOS_TCP_Reception.h"
#include "FreeRTOS_TCP_State_Handling.h"

void TCPHandleState_harness()
{
    struct xTCP_SEGMENT xSegment;
    struct xTCP_CONTROL_BLOCK xControlBlock;

    // Nondeterministic inputs
    xSegment.ucFlags = nondet_uint8_t();
    xSegment.usLength = nondet_uint16_t();
    xSegment.ulSequenceNumber = nondet_uint32_t();
    xSegment.ulAckNumber = nondet_uint32_t();
    xSegment.ulSequenceNumberHigh = nondet_uint32_t();
    xSegment.ulAckNumberHigh = nondet_uint32_t();
    xSegment.usSourcePort = nondet_uint16_t();
    xSegment.usDestinationPort = nondet_uint16_t();
    xSegment.pcData = (uint8_t *)nondet_pointer();
    xSegment.pucEthernetBuffer = (uint8_t *)nondet_pointer();

    xControlBlock.u.xTCP.usLocalPort = nondet_uint16_t();
    xControlBlock.u.xTCP.usRemotePort = nondet_uint16_t();
    xControlBlock.u.xTCP.ulLocalIP = nondet_uint32_t();
    xControlBlock.u.xTCP.ulRemoteIP = nondet_uint32_t();
    xControlBlock.u.xTCP.eTCPState = nondet_uint8_t();
    xControlBlock.u.xTCP.ulOurSequenceNumber = nondet_uint32_t();
    xControlBlock.u.xTCP.ulAcknowledgmentNumber = nondet_uint32_t();
    xControlBlock.u.xTCP.ulWindowSize = nondet_uint32_t();
    xControlBlock.u.xTCP.ulMSS = nondet_uint32_t();
    xControlBlock.u.xTCP.ulHighestRxSequence = nondet_uint32_t();
    xControlBlock.u.xTCP.ulHighestTxSequence = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Missing = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Sent = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Acknowledged = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Received = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_SequenceNumber = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_AckSequenceNumber = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_WindowSize = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Timeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitTime = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_MaxRetransmitCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_Delay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_MaxDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxCount = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxTimeout = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitDelay = nondet_uint32_t();
    xControlBlock.u.xTCP.ulFIN_RetransmitMaxDelay = nondet_uint32_t();

    // Constraints
    __CPROVER_assume(xSegment.usLength <= ipconfigNETWORK_MTU);
    __CPROVER_assume(xControlBlock.u.xTCP.eTCPState < eTCP_STATE_MAX);

    // Call function under test
    TCPHandleState(&xControlBlock, &xSegment);

    // Postconditions
    assert(xControlBlock.u.xTCP.eTCPState < eTCP_STATE_MAX);
    assert(xControlBlock.u.xTCP.ulOurSequenceNumber >= 0);
    assert(xControlBlock.u.xTCP.ulAcknowledgmentNumber >= 0);
    assert(xControlBlock.u.xTCP.ulHighestRxSequence >= 0);
    assert(xControlBlock.u.xTCP.ulHighestTxSequence >= 0);
}