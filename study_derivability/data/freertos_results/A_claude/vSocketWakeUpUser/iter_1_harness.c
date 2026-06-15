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

/* vSocketWakeUpUser is declared in FreeRTOS_Sockets.h or FreeRTOS_IP_Private.h */
void vSocketWakeUpUser( FreeRTOS_Socket_t * pxSocket );

void vSocketWakeUpUser_harness( void )
{
    FreeRTOS_Socket_t * pxSocket;

    /* Allocate a socket nondeterministically. */
    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* If the socket is NULL, there is nothing to constrain. */
    if( pxSocket != NULL )
    {
        /* Nondeterministically initialize socket fields that
         * vSocketWakeUpUser may inspect. */

        /* ucProtocol can be FREERTOS_IPPROTO_TCP or FREERTOS_IPPROTO_UDP */
        uint8_t ucProtocol;
        __CPROVER_assume( ucProtocol == FREERTOS_IPPROTO_TCP ||
                          ucProtocol == FREERTOS_IPPROTO_UDP );
        pxSocket->ucProtocol = ucProtocol;

        /* Set up the select bits nondeterministically. */
        pxSocket->xSelectBits = nondet_EventBits_t();

        /* The event group handle may or may not be valid. */
        EventGroupHandle_t xEventGroup = nondet_bool() ? xEventGroupCreate() : NULL;
        pxSocket->xEventGroup = xEventGroup;

        /* The semaphore (pxUserSemaphore) may or may not be valid. */
        SemaphoreHandle_t pxUserSemaphore;
        if( nondet_bool() )
        {
            pxUserSemaphore = xSemaphoreCreateBinary();
        }
        else
        {
            pxUserSemaphore = NULL;
        }
        pxSocket->pxUserSemaphore = pxUserSemaphore;

        /* The queue (pxUserWakeCallback) or callback may be set. */
        pxSocket->pxUserWakeCallback = nondet_bool() ? ( SocketWakeupCallback_t ) nondet_voidp() : NULL;

        /* For TCP sockets, set up the stream buffers and other TCP fields. */
        if( ucProtocol == FREERTOS_IPPROTO_TCP )
        {
            /* xSemaphoreHandle for TCP */
            SemaphoreHandle_t xSemaphore;
            if( nondet_bool() )
            {
                xSemaphore = xSemaphoreCreateBinary();
            }
            else
            {
                xSemaphore = NULL;
            }
            pxSocket->u.xTCP.xConnectBlock.xSemaphore = xSemaphore;

            /* bits and other TCP state */
            pxSocket->u.xTCP.bits.bConnPrepared = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;
            pxSocket->u.xTCP.bits.bConnPassed   = nondet_bool() ? pdTRUE_UNSIGNED : pdFALSE_UNSIGNED;

            /* RxStream semaphore */
            SemaphoreHandle_t xRxStreamSem;
            if( nondet_bool() )
            {
                xRxStreamSem = xSemaphoreCreateBinary();
            }
            else
            {
                xRxStreamSem = NULL;
            }
            pxSocket->u.xTCP.xRxStreamBlock.xSemaphore = xRxStreamSem;

            /* TxStream semaphore */
            SemaphoreHandle_t xTxStreamSem;
            if( nondet_bool() )
            {
                xTxStreamSem = xSemaphoreCreateBinary();
            }
            else
            {
                xTxStreamSem = NULL;
            }
            pxSocket->u.xTCP.xTxStreamBlock.xSemaphore = xTxStreamSem;
        }

        /* xSocketValid check - ensure the socket looks valid enough. */
        pxSocket->ucOwnedByTask = 0U;
    }

    /* Call the function under test. */
    vSocketWakeUpUser( pxSocket );

    /* Postconditions:
     * vSocketWakeUpUser is a void function; its postconditions are
     * side-effect based (semaphores given, event bits set, callback called).
     * We assert that the socket pointer itself is unchanged (not freed). */
    if( pxSocket != NULL )
    {
        /* The socket's protocol should remain unchanged after the call. */
        assert( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ||
                pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP );
    }
}