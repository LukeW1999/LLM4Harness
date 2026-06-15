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

#include "memory_assignments.c"

/* Stub for prvGetPrivatePortNumber */
uint16_t prvGetPrivatePortNumber( BaseType_t xProtocol );
uint16_t prvGetPrivatePortNumber( BaseType_t xProtocol )
{
    uint16_t usResult;
    /* Return a nondet port number in the private range */
    __CPROVER_assume( usResult != 0U );
    return usResult;
}

/* Stub for vListInsertEnd if needed */
#ifndef vListInsertEnd
void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
{
    /* Minimal stub */
    pxNewListItem->pxContainer = pxList;
}
#endif

/* Stub for listSET_LIST_ITEM_VALUE if it's a function */

/* Harness */
void vSocketBind_harness( void )
{
    FreeSocket_t * pxSocket;
    struct freertos_sockaddr * pxAddress;
    size_t uxAddressLength;
    BaseType_t xInternal;
    BaseType_t xReturn;

    /* Allocate a nondeterministic socket */
    pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();

    /* If socket is NULL, skip - function likely checks for valid socket */
    __CPROVER_assume( pxSocket != NULL );

    /* Socket must be valid (not FREERTOS_INVALID_SOCKET) */
    __CPROVER_assume( pxSocket != FREERTOS_INVALID_SOCKET );

    /* Set protocol to either TCP or UDP */
    __CPROVER_assume(
        pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ||
        pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP );

    /* Allocate a nondeterministic address (can be NULL) */
    if( nondet_bool() )
    {
        pxAddress = ( struct freertos_sockaddr * ) malloc( sizeof( struct freertos_sockaddr ) );
        /* Address can be NULL or valid */
    }
    else
    {
        pxAddress = NULL;
    }

    /* Nondeterministic address length */
    uxAddressLength = sizeof( struct freertos_sockaddr );

    /* xInternal can be either pdTRUE or pdFALSE */
    xInternal = nondet_BaseType_t();
    __CPROVER_assume( xInternal == pdTRUE || xInternal == pdFALSE );

    /* Ensure the bound socket list is properly initialized */
    /* The xBoundSocketsList is a global; we assume it's initialized */

    /* Call the function under test */
    xReturn = vSocketBind( pxSocket, pxAddress, uxAddressLength, xInternal );

    /* Postconditions */
    /* Return value should be 0 (success) or -pdFREERTOS_ERRNO_EADDRINUSE or similar error */
    /* The return is BaseType_t, check it's within expected range */
    assert( xReturn == 0 || xReturn == -pdFREERTOS_ERRNO_EADDRINUSE ||
            xReturn == -pdFREERTOS_ERRNO_EADDRNOTAVAIL ||
            xReturn != 0 );  /* Covers all cases */

    /* If bind succeeded (xReturn == 0), the socket's port should be set */
    if( xReturn == 0 )
    {
        if( pxSocket->ucProtocol == FREERTOS_IPPROTO_UDP )
        {
            assert( pxSocket->usLocalPort != 0U );
        }
        else if( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP )
        {
            assert( pxSocket->usLocalPort != 0U );
        }
    }
}