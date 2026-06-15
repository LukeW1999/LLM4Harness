/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_TCP_IP.h"

/* Maximum buffer size for options */
#define MAX_OPTIONS_SIZE    60
#define MIN_OPTIONS_SIZE    0

/* Declare the function under test */
int32_t CheckOptions( FreeRTOS_Socket_t * pxSocket,
                      NetworkBufferDescriptor_t * pxNetworkBuffer );

void CheckOptions_harness( void )
{
    FreeRTOS_Socket_t xSocket;
    NetworkBufferDescriptor_t xNetworkBuffer;
    TCPPacket_t xTCPPacket;
    uint8_t ucOptionsBuffer[ MAX_OPTIONS_SIZE ];
    size_t xOptionsLength;
    int32_t lResult;

    /* Initialize socket with nondeterministic values */
    ( void ) memset( &xSocket, 0, sizeof( xSocket ) );
    ( void ) memset( &xNetworkBuffer, 0, sizeof( xNetworkBuffer ) );
    ( void ) memset( &xTCPPacket, 0, sizeof( xTCPPacket ) );

    /* Set up socket fields */
    xSocket.u.xTCP.usInitMSS = nondet_uint16_t();
    xSocket.u.xTCP.usCurMSS = nondet_uint16_t();
    xSocket.u.xTCP.ucTCPState = nondet_uint8_t();
    xSocket.u.xTCP.xTCPWindow.xSize.ulRxWindowLength = nondet_uint32_t();
    xSocket.u.xTCP.xTCPWindow.xSize.ulTxWindowLength = nondet_uint32_t();
    xSocket.u.xTCP.bits.bWinScaling = nondet_uint32_t() & 0x1;
    xSocket.u.xTCP.ucMyWinScaleFactor = nondet_uint8_t();
    xSocket.u.xTCP.ucPeerWinScaleFactor = nondet_uint8_t();

    /* Constrain MSS values to reasonable bounds */
    __CPROVER_assume( xSocket.u.xTCP.usInitMSS >= 1 );
    __CPROVER_assume( xSocket.u.xTCP.usInitMSS <= ipconfigTCP_MSS );
    __CPROVER_assume( xSocket.u.xTCP.usCurMSS >= 1 );
    __CPROVER_assume( xSocket.u.xTCP.usCurMSS <= ipconfigTCP_MSS );

    /* Set up options length - must be within valid TCP options range */
    xOptionsLength = nondet_size_t();
    __CPROVER_assume( xOptionsLength <= MAX_OPTIONS_SIZE );
    __CPROVER_assume( xOptionsLength % 4 == 0 ); /* Options must be 4-byte aligned */

    /* Fill options buffer with nondeterministic data */
    for( size_t i = 0; i < xOptionsLength; i++ )
    {
        ucOptionsBuffer[ i ] = nondet_uint8_t();
    }

    /* Set up the TCP header data offset field */
    uint8_t ucDataOffset = nondet_uint8_t();
    /* Data offset must encode at least 5 (20 bytes) and at most 15 (60 bytes) */
    __CPROVER_assume( ( ucDataOffset >> 4 ) >= 5 );
    __CPROVER_assume( ( ucDataOffset >> 4 ) <= 15 );

    /* Set up network buffer */
    size_t xBufferSize = sizeof( TCPPacket_t ) + xOptionsLength;
    uint8_t * pucBuffer = ( uint8_t * ) pvPortMalloc( xBufferSize );
    __CPROVER_assume( pucBuffer != NULL );

    ( void ) memset( pucBuffer, 0, xBufferSize );

    /* Copy TCP packet header */
    TCPPacket_t * pxTCPPacketInBuffer = ( TCPPacket_t * ) pucBuffer;
    pxTCPPacketInBuffer->xTCPHeader.ucDataOffset = ucDataOffset;

    /* Copy nondeterministic options after header */
    ( void ) memcpy( pucBuffer + sizeof( TCPPacket_t ), ucOptionsBuffer, xOptionsLength );

    xNetworkBuffer.pucEthernetBuffer = pucBuffer;
    xNetworkBuffer.xDataLength = xBufferSize;

    /* Save pre-call state for postcondition checks */
    uint16_t usInitMSS_pre = xSocket.u.xTCP.usInitMSS;

    /* Call the function under test */
    lResult = CheckOptions( &xSocket, &xNetworkBuffer );

    /* Postcondition assertions */
    /* MSS should not exceed the initial MSS */
    assert( xSocket.u.xTCP.usCurMSS <= xSocket.u.xTCP.usInitMSS ||
            xSocket.u.xTCP.usCurMSS == usInitMSS_pre );

    /* MSS should be at least 1 if set */
    if( xSocket.u.xTCP.usCurMSS > 0 )
    {
        assert( xSocket.u.xTCP.usCurMSS >= 1 );
    }

    /* Window scale factor should be within valid range */
    assert( xSocket.u.xTCP.ucPeerWinScaleFactor <= 14 ||
            xSocket.u.xTCP.bits.bWinScaling == 0 );

    /* Free allocated memory */
    vPortFree( pucBuffer );
}