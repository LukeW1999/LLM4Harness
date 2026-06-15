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

BaseType_t vSocketBind( FreeRTOS_Socket_t * pxSocket,
                        const struct freertos_sockaddr * pxAddress,
                        socklen_t xAddressLength );

/* Abstraction of this functions allocate and return xWantedSize data. */
void * pvPortMallocLarge( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

FreeRTOS_Socket_t * ensure_FreeRTOS_Socket_t_is_allocated()
{
    FreeRTOS_Socket_t * pxSocket = ( FreeRTOS_Socket_t * ) safeMalloc( sizeof( FreeRTOS_Socket_t ) );
    __CPROVER_assume( pxSocket != NULL );
    return pxSocket;
}

struct freertos_sockaddr * ensure_freertos_sockaddr_is_allocated()
{
    struct freertos_sockaddr * pxAddress = ( struct freertos_sockaddr * ) safeMalloc( sizeof( struct freertos_sockaddr ) );
    __CPROVER_assume( pxAddress != NULL );
    return pxAddress;
}

void harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
    struct freertos_sockaddr * pxAddress = ensure_freertos_sockaddr_is_allocated();
    socklen_t xAddressLength;

    /* This function expects socket to be not NULL, as it has been validated previously */
    __CPROVER_assume( pxSocket != NULL );

    /* Assume address length to be in the range of maximum supported size. */
    __CPROVER_assume( xAddressLength > 0 && xAddressLength <= sizeof( struct freertos_sockaddr ) );

    /* Initialize the socket type and other necessary fields */
    pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;
    pxSocket->usLocalPort = 0; /* Port 0 indicates that a port number should be assigned */

    /* Call the function under test */
    vSocketBind( pxSocket, pxAddress, xAddressLength );
}