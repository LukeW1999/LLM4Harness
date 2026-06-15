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

void vSocketClose( FreeRTOS_Socket_t * pxSocket );

/* Abstraction of this functions allocate and return xWantedSize data. */
void * pvPortMallocLarge( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    BaseType_t xReturn;

    __CPROVER_assume( ( xReturn == pdTRUE ) || ( xReturn == pdFALSE ) );

    return xReturn;
}

void vTCPStateChange( FreeRTOS_Socket_t * pxSocket,
                      enum eTCP_STATE eTCPState )
{
}

void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
}

void vListInitialise( List_t * pxList )
{
}

void vListInsertEnd( List_t * pxList, ListItem_t * pxNewListItem )
{
}

void vListRemove( ListItem_t * pxItemToRemove )
{
}

void harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* This function expects socket to be not NULL, as it has been validated previously */
    __CPROVER_assume( pxSocket != NULL );

    /* Assume the socket is in a valid state */
    __CPROVER_assume( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP );

    /* Assume the socket is bound and connected */
    __CPROVER_assume( pxSocket->u.xTCP.eTCPState == eESTABLISHED );

    /* Assume the socket has valid stream buffers */
    pxSocket->u.xTCP.rxStream = safeMalloc( sizeof( StreamBuffer_t ) );
    __CPROVER_assume( pxSocket->u.xTCP.rxStream != NULL );
    pxSocket->u.xTCP.txStream = safeMalloc( sizeof( StreamBuffer_t ) );
    __CPROVER_assume( pxSocket->u.xTCP.txStream != NULL );

    /* Assume the socket has valid network buffer list */
    vListInitialise( &pxSocket->u.xTCP.rxWaitingHead );
    vListInitialise( &pxSocket->u.xTCP.txWaitingHead );

    vSocketClose( pxSocket );
}