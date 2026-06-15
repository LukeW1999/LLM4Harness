/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"

/* CBMC includes. */
#include "memory_assignments.c"

/****************************************************************
* Signature of function under test
****************************************************************/

BaseType_t vSocketBind( FreeRTOS_Socket_t * pxSocket,
                        struct freertos_sockaddr * pxBindAddress,
                        size_t uxAddressLength,
                        BaseType_t xInternal );

/****************************************************************
* Stubs / abstractions
****************************************************************/

/* Stub for the list-based port lookup used inside vSocketBind. */
FreeRTOS_Socket_t * __CPROVER_file_local_FreeRTOS_Sockets_c_pxListFindListItemWithValue(
    const List_t * pxList,
    TickType_t xWantedItemValue )
{
    FreeRTOS_Socket_t * pxReturn;
    return pxReturn;
}

/* Stub for the random number generator used to pick an ephemeral port. */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    BaseType_t xReturn;
    uint32_t ulValue;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    if( xReturn == pdTRUE )
    {
        *pulNumber = ulValue;
    }

    return xReturn;
}

/* Stub for scheduler-state query used in socket list operations. */
BaseType_t xTaskGetSchedulerState( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == taskSCHEDULER_NOT_STARTED ) ||
                      ( xReturn == taskSCHEDULER_SUSPENDED )    ||
                      ( xReturn == taskSCHEDULER_RUNNING ) );
    return xReturn;
}

/* Stub for the event-group / semaphore operations used by the IP-task
   synchronisation inside vSocketBind. */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t uxTimeout )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

/* Stub for the IP-task check used for internal binds. */
BaseType_t xIsCallingFromIPTask( void )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

/* List stubs – keep list operations side-effect-free for the proof. */
void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
{
}

void vListInsert( List_t * const pxList,
                  ListItem_t * const pxNewListItem )
{
}

UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
    UBaseType_t uxReturn;
    return uxReturn;
}

/* Semaphore / mutex stubs. */
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore,
                           TickType_t xBlockTime )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore )
{
    BaseType_t xReturn;
    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );
    return xReturn;
}

/****************************************************************
* Harness
****************************************************************/

void vSocketBind_harness()
{
    FreeRTOS_Socket_t * pxSocket;
    struct freertos_sockaddr * pxBindAddress;
    size_t uxAddressLength;
    BaseType_t xInternal;

    /* Allocate and initialise the socket under test. */
    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* vSocketBind requires a non-NULL socket. */
    __CPROVER_assume( pxSocket != NULL );

    /* The socket must be either UDP or TCP – other values are not meaningful. */
    __CPROVER_assume( ( pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP ) ||
                      ( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ) );

    /* Allocate a bind address – may be NULL (auto-bind) or a valid struct. */
    pxBindAddress = ( struct freertos_sockaddr * ) safeMalloc( sizeof( struct freertos_sockaddr ) );

    /* uxAddressLength is passed in but primarily used for size checks; keep
       it in a sensible range. */
    __CPROVER_assume( uxAddressLength <= sizeof( struct freertos_sockaddr ) );

    /* xInternal selects whether the caller is the IP task itself. */
    __CPROVER_assume( ( xInternal == pdTRUE ) || ( xInternal == pdFALSE ) );

    /* Call the function under verification. */
    ( void ) vSocketBind( pxSocket, pxBindAddress, uxAddressLength, xInternal );
}