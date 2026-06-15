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

BaseType_t TCPAddRxdata( struct xNetworkBufferDescriptor * pxNetworkBuffer );

void TCPAddRxdata_harness()
{
    struct xNetworkBufferDescriptor * pxNetworkBuffer;
    size_t uxPayloadLength;

    pxNetworkBuffer = safeMalloc( sizeof( struct xNetworkBufferDescriptor ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = safeMalloc( ipconfigNETWORK_MTU );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    __CPROVER_assume( uxPayloadLength > 0 && uxPayloadLength < ipconfigNETWORK_MTU );
    pxNetworkBuffer->xDataLength = uxPayloadLength;

    TCPAddRxdata( pxNetworkBuffer );

    /* Postconditions */
    __CPROVER_assert( pxNetworkBuffer->xDataLength <= ipconfigNETWORK_MTU, "Data length should not exceed MTU" );
}