/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

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

/* CBMC includes. */
#include <assert.h>

/* The function under test. */
BaseType_t CheckOptionsOuter( FreeRTOS_Socket_t * pxSocket,
                              NetworkBufferDescriptor_t * pxNetworkBuffer );

/* Maximum size constraints for bounded model checking. */
#define MAX_PACKET_SIZE    1500
#define MIN_PACKET_SIZE    ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER )

void CheckOptionsOuter_harness( void )
{
    FreeRTOS_Socket_t * pxSocket;
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t bufferSize;
    BaseType_t xResult;

    /* Allocate socket nondeterministically. */
    pxSocket = ( FreeRTOS_Socket_t * ) malloc( sizeof( FreeRTOS_Socket_t ) );
    __CPROVER_assume( pxSocket != NULL );

    /* Initialize socket with nondeterministic values. */
    pxSocket->u.xTCP.eTCPState = ( eIPTCPState_t ) nondet_uint8_t();
    pxSocket->u.xTCP.ucTCPState = nondet_uint8_t();
    pxSocket->u.xTCP.bits.bMssChange = nondet_uint32_t() & 0x1;
    pxSocket->u.xTCP.bits.bWinScaling = nondet_uint32_t() & 0x1;
    pxSocket->u.xTCP.usMSS = nondet_uint16_t();
    pxSocket->u.xTCP.usInitMSS = nondet_uint16_t();
    pxSocket->u.xTCP.ucMyWinScaleFactor = nondet_uint8_t();
    pxSocket->u.xTCP.ucPeerWinScaleFactor = nondet_uint8_t();
    pxSocket->u.xTCP.xTCPWindow.xSize.ulRxWindowLength = nondet_uint32_t();
    pxSocket->u.xTCP.xTCPWindow.xSize.ulTxWindowLength = nondet_uint32_t();

    /* Allocate network buffer nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Choose a nondeterministic buffer size within valid bounds. */
    bufferSize = nondet_size_t();
    __CPROVER_assume( bufferSize >= MIN_PACKET_SIZE );
    __CPROVER_assume( bufferSize <= MAX_PACKET_SIZE );

    /* Allocate the network buffer data. */
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( bufferSize );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
    pxNetworkBuffer->xDataLength = bufferSize;

    /* Set up the IP header within the buffer. */
    ProtocolPacket_t * pxPacket = ( ProtocolPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Set IP header fields to nondeterministic values with constraints. */
    uint8_t ucVersionHeaderLength = nondet_uint8_t();
    /* IP version must be IPv4 (4) and header length must be valid. */
    __CPROVER_assume( ( ucVersionHeaderLength >> 4 ) == 4 );
    /* Header length in 32-bit words: minimum 5, maximum 15. */
    uint8_t ucHeaderLen = ucVersionHeaderLength & 0x0F;
    __CPROVER_assume( ucHeaderLen >= 5 );
    __CPROVER_assume( ucHeaderLen <= 15 );

    pxPacket->xTCPPacket.xIPHeader.ucVersionHeaderLength = ucVersionHeaderLength;

    /* Total length of IP packet. */
    uint16_t usTotalLength = nondet_uint16_t();
    __CPROVER_assume( usTotalLength >= ( ucHeaderLen * 4 + ipSIZE_OF_TCP_HEADER ) );
    __CPROVER_assume( usTotalLength <= ( uint16_t )( bufferSize - ipSIZE_OF_ETH_HEADER ) );
    pxPacket->xTCPPacket.xIPHeader.usLength = FreeRTOS_htons( usTotalLength );

    /* Set TCP header data offset field. */
    uint8_t ucTCPOffset = nondet_uint8_t();
    /* TCP data offset must be at least 5 (minimum TCP header size). */
    __CPROVER_assume( ( ucTCPOffset >> 4 ) >= 5 );
    pxPacket->xTCPPacket.xTCPHeader.ucDataOffset = ucTCPOffset;

    /* Set TCP flags. */
    pxPacket->xTCPPacket.xTCPHeader.ucTCPFlags = nondet_uint8_t();

    /* Set TCP sequence and acknowledgement numbers. */
    pxPacket->xTCPPacket.xTCPHeader.ulSequenceNumber = nondet_uint32_t();
    pxPacket->xTCPPacket.xTCPHeader.ulAckNr = nondet_uint32_t();

    /* Set source and destination ports. */
    pxPacket->xTCPPacket.xTCPHeader.usSourcePort = nondet_uint16_t();
    pxPacket->xTCPPacket.xTCPHeader.usDestinationPort = nondet_uint16_t();

    /* Set window size. */
    pxPacket->xTCPPacket.xTCPHeader.usWindow = nondet_uint16_t();

    /* Call the function under test. */
    xResult = CheckOptionsOuter( pxSocket, pxNetworkBuffer );

    /* Assert postconditions:
     * The result should be either pdPASS or pdFAIL (i.e., a valid BaseType_t value).
     * In FreeRTOS, pdPASS == pdTRUE == 1, pdFAIL == pdFALSE == 0.
     */
    assert( ( xResult == pdPASS ) || ( xResult == pdFAIL ) );
}