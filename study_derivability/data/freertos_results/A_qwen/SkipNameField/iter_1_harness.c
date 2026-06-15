#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"

void SkipNameField_harness()
{
    uint8_t *pxBuffer;
    size_t uxBufferLength;
    size_t uxOffset;

    pxBuffer = (uint8_t *)malloc(nondet_uint());
    uxBufferLength = nondet_size_t();
    uxOffset = nondet_size_t();

    __CPROVER_assume(pxBuffer != NULL);
    __CPROVER_assume(uxBufferLength > 0);
    __CPROVER_assume(uxOffset < uxBufferLength);

    // Initialize buffer with nondeterministic values
    for (size_t i = 0; i < uxBufferLength; i++) {
        pxBuffer[i] = nondet_uint8();
    }

    size_t uxResult = SkipNameField(pxBuffer, uxBufferLength, uxOffset);

    // Postconditions
    assert(uxResult <= uxBufferLength);
    assert(uxResult >= uxOffset);
}