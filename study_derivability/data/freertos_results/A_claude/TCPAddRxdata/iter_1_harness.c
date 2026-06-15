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
uint32_t TCPAddRxdata( FreeRTOS_Socket_t * pxSocket,
                       uint32_t ulSequenceNumber,
                       uint8_t * pcData,
                       uint32_t ulByteCount );

/****************************************************************
* Proof harness
****************************************************************/
void TCPAddRxdata_harness( void )
{
    FreeRTOS_Socket_t * pxSocket;
    uint32_t ulSequenceNumber;
    uint8_t * pcData;
    uint32_t ulByteCount;

    /* Allocate a socket nondeterministically. */
    pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );
    __CPROVER_assume( pxSocket != NULL );

    /* The socket must be a valid TCP socket. */
    pxSocket->ucProtocol = FREERTOS_IPPROTO_TCP;

    /* Set up the stream buffer for receiving data. */
    StreamBuffer_t * pxStream = ( StreamBuffer_t * ) malloc( sizeof( StreamBuffer_t ) + ipconfigTCP_MSS );
    __CPROVER_assume( pxStream != NULL );

    /* Initialize stream buffer length nondeterministically but within bounds. */
    pxStream->LENGTH = nondet_uint32_t();
    __CPROVER_assume( pxStream->LENGTH > 0 );
    __CPROVER_assume( pxStream->LENGTH <= ipconfigTCP_MSS );

    pxStream->uxHead = nondet_size_t();
    __CPROVER_assume( pxStream->uxHead < pxStream->LENGTH );

    pxStream->uxMid = nondet_size_t();
    __CPROVER_assume( pxStream->uxMid < pxStream->LENGTH );

    pxStream->uxTail = nondet_size_t();
    __CPROVER_assume( pxStream->uxTail < pxStream->LENGTH );

    pxSocket->u.xTCP.rxStream = pxStream;

    /* Set up the TCP window. */
    pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.rx.ulHighestSequenceNumber = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.rx.ulFINSequenceNumber = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = nondet_uint32_t();

    /* Set nondeterministic sequence number. */
    ulSequenceNumber = nondet_uint32_t();

    /* Allocate a nondeterministic data buffer. */
    ulByteCount = nondet_uint32_t();
    __CPROVER_assume( ulByteCount <= ipconfigTCP_MSS );

    if( ulByteCount > 0 )
    {
        pcData = ( uint8_t * ) malloc( ulByteCount );
        __CPROVER_assume( pcData != NULL );
    }
    else
    {
        pcData = NULL;
    }

    /* Set additional TCP socket fields nondeterministically. */
    pxSocket->u.xTCP.bits.bMallocError = pdFALSE_UNSIGNED;
    pxSocket->u.xTCP.usCurMSS = nondet_uint16_t();
    __CPROVER_assume( pxSocket->u.xTCP.usCurMSS > 0 );
    __CPROVER_assume( pxSocket->u.xTCP.usCurMSS <= ipconfigTCP_MSS );

    pxSocket->u.xTCP.xTCPWindow.xSize.ulRxWindowLength = nondet_uint32_t();
    __CPROVER_assume( pxSocket->u.xTCP.xTCPWindow.xSize.ulRxWindowLength > 0 );
    __CPROVER_assume( pxSocket->u.xTCP.xTCPWindow.xSize.ulRxWindowLength <= ipconfigTCP_MSS );

    /* Call the function under test. */
    uint32_t ulResult = TCPAddRxdata( pxSocket, ulSequenceNumber, pcData, ulByteCount );

    /* Postconditions:
     * The return value must be at most ulByteCount bytes added. */
    assert( ulResult <= ulByteCount );
}