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
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv6_Utils.h"

/* CBMC includes. */
#include "cbmc.h"

/* Declaration of the function under test */
BaseType_t prvChecksumIPv6Checks( uint8_t * pucEthernetBuffer,
                                   size_t uxBufferLength,
                                   struct xPacketSummary * pxSet );

void harness()
{
    size_t uxBufferLength;
    uint8_t * pucEthernetBuffer;
    struct xPacketSummary xSet;

    /* Nondeterministic buffer length */
    __CPROVER_assume( uxBufferLength >= sizeof( EthernetHeader_t ) + ipSIZE_OF_IPv6_HEADER );
    __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU + sizeof( EthernetHeader_t ) );

    /* Allocate nondeterministic ethernet buffer */
    pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Set up the packet summary with nondeterministic values */
    /* pxIPPacket_IPv6 points into the ethernet buffer */
    xSet.pxIPPacket_IPv6 = ( IPPacket_IPv6_t * ) pucEthernetBuffer;

    /* Nondeterministic protocol length fields */
    uint16_t usPayloadLength;
    xSet.usPayloadLength = nondet_uint16_t();

    /* uxIPHeaderLength for IPv6 is fixed */
    xSet.uxIPHeaderLength = ipSIZE_OF_IPv6_HEADER;

    /* Nondeterministic protocol */
    xSet.ucProtocol = nondet_uint8_t();

    /* xIsIPv6 flag */
    xSet.xIsIPv6 = pdTRUE;

    /* Set pointer to source/destination addresses */
    xSet.pxSourceAddress = NULL;
    xSet.pxDestinationAddress = NULL;

    /* Additional fields that may be checked */
    xSet.usChecksum = nondet_uint16_t();
    xSet.iReturn = nondet_int();

    /* Call the function under test */
    BaseType_t xReturn = prvChecksumIPv6Checks( pucEthernetBuffer,
                                                 uxBufferLength,
                                                 &xSet );

    /* Assert postconditions:
     * The return value should be a valid BaseType_t value.
     * Typically prvChecksumIPv6Checks returns ipCORRECT_CRC (0) or an error code. */
    assert( xReturn == ipCORRECT_CRC ||
            xReturn == ipINVALID_LENGTH ||
            xReturn == ipUNHANDLED_PROTOCOL ||
            xReturn == pdFALSE ||
            xReturn == pdTRUE ||
            xReturn >= 0 );

    /* Free the allocated buffer */
    free( pucEthernetBuffer );
}