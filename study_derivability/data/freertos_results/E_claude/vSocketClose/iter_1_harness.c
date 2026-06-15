```c
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"

/* CBMC includes. */
#include "memory_assignments.c"

/****************************************************************
* Signature of function under test
****************************************************************/

void vSocketClose( FreeRTOS_Socket_t * pxSocket );

/****************************************************************
* Stubs / abstractions
****************************************************************/

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

void vTCPStateChange( FreeRTOS_Socket_t * pxSocket,
                      enum eTCP_STATE eTCPState )
{
}

BaseType_t xTCPSocketCheck( FreeRTOS_Socket_t * pxSocket )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit,
                                 const BaseType_t xWaitForAllBits,
                                 TickType_t xTicksToWait )
{
    EventBits_t uxReturn;
    return uxReturn;
}

EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet )
{
    EventBits_t uxReturn;
    return uxReturn;
}

void vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
}

EventGroupHandle_t xEventGroupCreate( void )
{
    EventGroupHandle_t xReturn;
    return xReturn;
}

void * pvPortMalloc( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

void vPortFree( void * pv )
{
    /* Allow freeing of memory. */
}

void * pvPortMallocLarge( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

void vPortFreeLarge( void * pv )
{
    /* Allow freeing of memory. */
}

BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

void vTaskSuspendAll( void )
{
}

BaseType_t xTaskResumeAll( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

BaseType_t xTaskGetSchedulerState( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == taskSCHEDULER_RUNNING ) ||
                      ( xReturn == taskSCHEDULER_NOT_STARTED ) ||
                      ( xReturn == taskSCHEDULER_SUSPENDED ) );
    return xReturn;
}

void * xQueueCreateMutex( uint8_t ucQueueType )
{
    void * pvReturn;
    return pvReturn;
}

BaseType_t xQueueSemaphoreTake( QueueHandle_t xQueue,
                                TickType_t xTicksToWait )
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

void vQueueDelete( QueueHandle_t xQueue )
{
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn;
    return uxReturn;
}

void FreeRTOS_debug_printf( const char * pcFormatString, ... )
{
}

BaseType_t listLIST_IS_EMPTY( const List_t * pxList )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

void vListInitialise( List_t * const pxList )
{
}

void vListInitialiseItem( ListItem_t * const pxItem )
{
}

void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
{
}

UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
    UBaseType_t uxReturn;
    return uxReturn;
}

/****************************************************************
* Harness
****************************************************************/

void vSocketClose_harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* vSocketClose expects a non-NULL socket. */
    __CPROVER_assume( pxSocket != NULL );

    /* The socket must have a valid IPTYPE. */
    __CPROVER_assume( ( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP ) ||
                      ( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_UDP ) );

#if ipconfigUSE_TCP == 1
    if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP )
    {
        /* Assume valid stream sizes for TCP sockets. */
        __CPROVER_assume( pxSocket->u.xTCP.uxRxStreamSize >= 0 &&
                          pxSocket->u.xTCP.uxRxStreamSize <= ipconfigTCP_RX_BUFFER_LENGTH );
        __CPROVER_assume( pxSocket->u.xTCP.uxTxStreamSize >= 0 &&
                          pxSocket->u.xTCP.uxTxStreamSize <= ipconfigTCP_TX_BUFFER_LENGTH );

        /* TCP streams may or may not be allocated; set them to NULL or valid pointer. */
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.rxStream = safeMalloc( sizeof( StreamBuffer_t ) );
        }
        else
        {
            px