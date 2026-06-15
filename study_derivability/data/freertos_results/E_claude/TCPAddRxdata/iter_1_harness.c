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
#include "FreeRTOS_TCP_Reception.h"

/* CBMC includes. */
#include "memory_assignments.c"

/****************************************************************
* Signature of function under test
****************************************************************/

BaseType_t TCPAddRxdata( FreeRTOS_Socket_t * pxSocket,
                         const uint8_t * pcData,
                         uint32_t ulByteCount );

/****************************************************************
* Helper: allocate and initialize a TCP socket with valid
* stream-buffer state for CBMC exploration.
****************************************************************/

static FreeRTOS_Socket_t * allocate_and_init_socket( void )
{
    FreeRTOS_Socket_t * pxSocket = safeMalloc( sizeof( FreeRTOS_Socket_t ) );

    if( pxSocket == NULL )
    {
        return NULL;
    }

    /* Initialise the whole structure to zero first. */
    __CPROVER_assume( pxSocket != NULL );

    /* The Rx stream buffer may or may not be present. */
    if( nondet_bool() )
    {
        pxSocket->u.xTCP.rxStream = safeMalloc( sizeof( StreamBuffer_t ) );

        if( pxSocket->u.xTCP.rxStream != NULL )
        {
            /* Give the stream buffer a non-zero length so accesses are valid. */
            uint32_t uxLength;
            __CPROVER_assume( uxLength > 0 && uxLength <= ipconfigTCP_RX_BUFFER_LENGTH );
            pxSocket->u.xTCP.rxStream->LENGTH = uxLength;

            /* Head, tail, front must be within [0, LENGTH). */
            __CPROVER_assume( pxSocket->u.xTCP.rxStream->uxHead  < uxLength );
            __CPROVER_assume( pxSocket->u.xTCP.rxStream->uxTail  < uxLength );
            __CPROVER_assume( pxSocket->u.xTCP.rxStream->uxFront < uxLength );
        }
    }
    else
    {
        pxSocket->u.xTCP.rxStream = NULL;
    }

    /* uxRxWinSize is used to compute the Rx window; keep it bounded. */
    __CPROVER_assume( pxSocket->u.xTCP.uxRxWinSize > 0 &&
                      pxSocket->u.xTCP.uxRxWinSize <= ( uint32_t ) ipconfigTCP_RX_BUFFER_LENGTH );

    /* Sliding-window state. */
    __CPROVER_assume( pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber <=
                      pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber );

    return pxSocket;
}

/****************************************************************
* Proof harness
****************************************************************/

void harness()
{
    FreeRTOS_Socket_t * pxSocket;
    const uint8_t * pcData;
    uint32_t ulByteCount;

    /* Allocate and set up the socket. */
    pxSocket = allocate_and_init_socket();
    __CPROVER_assume( pxSocket != NULL );

    /* The data pointer may be NULL (zero-byte copy) or point to a buffer. */
    if( nondet_bool() && ulByteCount > 0 )
    {
        __CPROVER_assume( ulByteCount > 0 && ulByteCount <= ipconfigNETWORK_MTU );
        pcData = safeMalloc( ulByteCount );
        __CPROVER_assume( pcData != NULL );
    }
    else
    {
        /* ulByteCount == 0 is a valid edge-case; pcData may be NULL. */
        ulByteCount = 0;
        pcData = NULL;
    }

    /* Call the function under test. */
    ( void ) TCPAddRxdata( pxSocket, pcData, ulByteCount );
}