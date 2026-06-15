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

BaseType_t RecvFrom_CopyPacket( FreeRTOS_Socket_t * pxSocket,
                                NetworkBufferDescriptor_t * pxNetworkBuffer,
                                BaseType_t lFlags );

/* Abstraction of this functions allocate and return xWantedSize data. */
void * pvPortMallocLarge( size_t xWantedSize )
{
    return safeMalloc( xWantedSize );
}

NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxReturn;

    return pxReturn;
}

void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
}

void harness()
{
    FreeRTOS_Socket_t * pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
    NetworkBufferDescriptor_t * pxNetworkBuffer = ensure_NetworkBufferDescriptor_t_is_allocated();
    BaseType_t lFlags;

    /* This function expects socket to be not NULL, as it has been validated previously */
    __CPROVER_assume( pxSocket != NULL );

    /* Assume size of streams to be in the range of maximum supported size.*/
    __CPROVER_assume( pxSocket->u.xTCP.uxRxStreamSize >= 0 && pxSocket->u.xTCP.uxRxStreamSize < ipconfigTCP_RX_BUFFER_LENGTH );
    __CPROVER_assume( pxSocket->u.xTCP.uxTxStreamSize >= 0 && pxSocket->u.xTCP.uxTxStreamSize < ipconfigTCP_TX_BUFFER_LENGTH );

    /* ipconfigTCP_MSS is guaranteed not less than tcpMINIMUM_SEGMENT_LENGTH by FreeRTOSIPConfigDefaults.h */
    __CPROVER_assume( pxSocket->u.xTCP.usMSS >= tcpMINIMUM_SEGMENT_LENGTH );

    /* Assume network buffer is not NULL */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Assume network buffer has valid data length */
    __CPROVER_assume( pxNetworkBuffer->xDataLength > 0U && pxNetworkBuffer->xDataLength <= ipconfigNETWORK_MTU );

    /* Assume flags are within a reasonable range */
    __CPROVER_assume( lFlags >= 0 && lFlags <= 0xFF );

    RecvFrom_CopyPacket( pxSocket, pxNetworkBuffer, lFlags );
}