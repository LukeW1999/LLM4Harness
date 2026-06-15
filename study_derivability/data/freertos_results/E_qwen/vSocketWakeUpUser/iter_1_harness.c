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

void vSocketWakeUpUser( FreeRTOS_Socket_t * pxSocket );

/* Abstraction of this functions allocate and return xWantedSize data. */
void * pvPortMallocLarge( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

void harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* This function expects socket to be not NULL, as it has been validated previously */
    __CPROVER_assume( pxSocket != NULL );

    /* Assume valid state for the socket */
    __CPROVER_assume( pxSocket->u.xTCP.eTCPState >= eCLOSED && pxSocket->u.xTCP.eTCPState <= eFIN_WAIT_2 );

    /* Assume valid flags for the socket */
    __CPROVER_assume( pxSocket->bits.bIsBound == pdTRUE || pxSocket->bits.bIsBound == pdFALSE );
    __CPROVER_assume( pxSocket->bits.bWasConnected == pdTRUE || pxSocket->bits.bWasConnected == pdFALSE );
    __CPROVER_assume( pxSocket->bits.bPassAccept == pdTRUE || pxSocket->bits.bPassAccept == pdFALSE );

    /* Assume valid event group handle */
    __CPROVER_assume( pxSocket->xEventGroup != NULL );

    vSocketWakeUpUser( pxSocket );
}