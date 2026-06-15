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

void __CPROVER_file_local_FreeRTOS_Sockets_c_vSocketWakeUpUser( FreeRTOS_Socket_t * pxSocket );

/****************************************************************
* Stubs
****************************************************************/

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify,
                             BaseType_t * const pxHigherPriorityTaskWoken )
{
    /* Stub: no-op */
}

BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify,
                               UBaseType_t uxIndexToNotify,
                               uint32_t ulValue,
                               eNotifyAction eAction,
                               uint32_t * pulPreviousNotificationValue )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

void harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* The function expects socket to be not NULL */
    __CPROVER_assume( pxSocket != NULL );

    /* The event group handle can be NULL or non-NULL */
    /* pxSocket->xEventGroup is already part of the allocated structure */

    /* The task waiting on the socket can be NULL or a valid task handle */
    /* pxSocket->pxUserWakeCallback can be NULL or a valid callback */

    /* Constrain socket type to valid values */
    __CPROVER_assume( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ||
                      pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP );

    __CPROVER_file_local_FreeRTOS_Sockets_c_vSocketWakeUpUser( pxSocket );
}