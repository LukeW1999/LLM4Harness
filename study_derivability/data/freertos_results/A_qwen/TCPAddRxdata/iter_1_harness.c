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
BaseType_t TCPAddRxdata( FreeRTOS_Socket_t *pxSocket, const uint8_t *pcData, size_t uxOffset, size_t uxByteCount );

/****************************************************************
* Harness function
****************************************************************/
void TCPAddRxdata_harness()
{
    /* Nondeterministic inputs */
    FreeRTOS_Socket_t *pxSocket = (FreeRTOS_Socket_t *)malloc(sizeof(FreeRTOS_Socket_t));
    uint8_t *pcData = (uint8_t *)malloc(nondet_size_t());
    size_t uxOffset = nondet_size_t();
    size_t uxByteCount = nondet_size_t();

    /* Constraints */
    __CPROVER_assume(pxSocket != NULL);
    __CPROVER_assume(pcData != NULL || uxByteCount == 0);
    __CPROVER_assume(uxOffset <= pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber);
    __CPROVER_assume(uxByteCount <= pxSocket->u.xTCP.xTCPWindow.rx.ulWindowSize);

    /* Initialize socket fields used in the function */
    pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = uxOffset + uxByteCount;
    pxSocket->u.xTCP.xTCPWindow.rx.ulWindowSize = uxByteCount;
    pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber = uxOffset + uxByteCount;

    /* Call function under test */
    BaseType_t result = TCPAddRxdata(pxSocket, pcData, uxOffset, uxByteCount);

    /* Postconditions */
    assert(result == pdPASS || result == pdFAIL);
    if (result == pdPASS) {
        assert(pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber == uxOffset + uxByteCount);
        assert(pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber >= uxOffset + uxByteCount);
    }
}