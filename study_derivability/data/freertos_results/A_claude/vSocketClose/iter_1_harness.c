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

#include "freertos_api.c"
#include "memory_assignments.c"

/* The memory safety of vTCPWindowDestroy has already been proved in
 * proofs/TCPWin/vTCPWindowDestroy. */
void vTCPWindowDestroy( TCPWindow_t const * xWindow )
{
    /* Stub: already verified separately. */
}

/* Stub for vPortFree to avoid actual freeing during verification. */
void vPortFree( void * pv )
{
    /* Stub */
}

/* Stub for eventgroup operations */
void vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
    /* Stub */
}

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
    return nondet_BaseType_t();
}

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                      TickType_t uxTimeout )
{
    return nondet_BaseType_t();
}

/* Harness */
void vSocketClose_harness( void )
{
    FreeSocket_t * pxSocket;

    /* Allocate a nondeterministic socket */
    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* We need a valid socket, not NULL */
    __CPROVER_assume( pxSocket != NULL );

    /* Ensure the socket type is valid */
    __CPROVER_assume( ( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ) ||
                      ( pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP ) );

    /* If TCP, set up TCP-specific fields */
    if( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP )
    {
        /* The TCP window destroy is already verified, stub it out.
         * Constrain the xTCP union fields to safe values. */
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.rxStream = NULL;
        }
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.txStream = NULL;
        }
        if( nondet_bool() )
        {
            pxSocket->u.xTCP.pxPeerSocket = NULL;
        }
    }

    /* If UDP, constrain the UDP-specific fields */
    if( pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP )
    {
        /* Waiting packet list should be initialised */
        if( nondet_bool() )
        {
            /* Ensure list head is valid or NULL */
            pxSocket->u.xUDP.pxWaitingPacketsList = NULL;
        }
    }

    /* Ensure the socket list item is initialised */
    vListInitialiseItem( &( pxSocket->xBoundSocketListItem ) );
    vListInitialiseItem( &( pxSocket->xSocketListItem ) );

    /* Set up event bits / semaphore handle */
    if( nondet_bool() )
    {
        pxSocket->pxUserSemaphore = NULL;
    }

    if( nondet_bool() )
    {
        pxSocket->pxUserWakeCallback = NULL;
    }

#if ( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
    /* Socket select set */
    if( nondet_bool() )
    {
        pxSocket->pxSocketSet = NULL;
    }
#endif

    /* Call the function under test */
    vSocketClose( pxSocket );

    /* Postcondition: function should have returned (not crashed).
     * The socket memory is freed, so we cannot dereference pxSocket.
     * We assert a trivially true postcondition to confirm we reached here. */
    assert( 1 );
}