/* Standard includes. */
#include <stdint.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"

void ReadNameField_harness()
{
    uint8_t *pucPtr;
    const uint8_t *pucUDPPayloadBase;
    size_t uxRemaining;
    size_t uxOutLength;
    char *pcName;

    pucPtr = (uint8_t *)malloc(nondet_uint());
    pucUDPPayloadBase = (const uint8_t *)malloc(nondet_uint());
    uxRemaining = nondet_size_t();
    uxOutLength = nondet_size_t();
    pcName = (char *)malloc(nondet_uint());

    __CPROVER_assume(pucPtr != NULL);
    __CPROVER_assume(pucUDPPayloadBase != NULL);
    __CPROVER_assume(pcName != NULL);
    __CPROVER_assume(uxRemaining > 0);
    __CPROVER_assume(uxOutLength > 0);

    ReadNameField(&pucPtr, pucUDPPayloadBase, uxRemaining, uxOutLength, pcName);

    assert(pucPtr != NULL);
    assert(pcName != NULL);
    assert(uxRemaining <= uxOutLength);
}