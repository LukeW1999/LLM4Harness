/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "memory_assignments.c"

void vSocketWakeUpUser_harness()
{
    Socket_t xSocket;
    BaseType_t xResult;

    /* Allocate nondeterministic inputs */
    xSocket = (Socket_t)nondet_pointer();

    /* Constrain inputs */
    __CPROVER_assume(xSocket != NULL);
    __CPROVER_assume(xSocket->ucProtocol >= 0 && xSocket->ucProtocol <= PROTOCOL_MAX);
    __CPROVER_assume(xSocket->xEventGroup != NULL);

    /* Call the function under test */
    xResult = vSocketWakeUpUser( xSocket );

    /* Assert postconditions */
    assert(xResult == pdTRUE || xResult == pdFALSE);
}