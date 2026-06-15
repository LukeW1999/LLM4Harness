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

int32_t __CPROVER_file_local_FreeRTOS_Sockets_c_RecvFrom_CopyPacket( uint8_t * pucEthernetBuffer,
                                                                      void * pvBuffer,
                                                                      size_t uxBufferLength,
                                                                      BaseType_t xFlags,
                                                                      struct freertos_sockaddr * pxSourceAddress,
                                                                      socklen_t * pxSourceAddressLength );

void harness()
{
    uint8_t * pucEthernetBuffer;
    void * pvBuffer;
    size_t uxBufferLength;
    BaseType_t xFlags;
    struct freertos_sockaddr * pxSourceAddress;
    socklen_t * pxSourceAddressLength;

    /* Allocate a NetworkBufferDescriptor with associated ethernet buffer.
     * The ethernet buffer must be large enough to hold at minimum a UDP packet header. */
    size_t uxEthernetBufferSize;
    __CPROVER_assume( uxEthernetBufferSize >= sizeof( UDPPacket_t ) );
    __CPROVER_assume( uxEthernetBufferSize < ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pucEthernetBuffer = safeMalloc( uxEthernetBufferSize );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* The buffer length can be anything from 0 to a large value. */
    __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU );

    /* pvBuffer can be NULL (zero-copy flag) or a valid buffer. */
    if( nondet_bool() )
    {
        pvBuffer = NULL;
    }
    else
    {
        pvBuffer = safeMalloc( uxBufferLength );
        /* pvBuffer may be NULL if uxBufferLength is 0 or allocation fails */
    }

    /* pxSourceAddress can be NULL or a valid address structure. */
    if( nondet_bool() )
    {
        pxSourceAddress = NULL;
    }
    else
    {
        pxSourceAddress = ( struct freertos_sockaddr * ) safeMalloc( sizeof( struct freertos_sockaddr ) );
    }

    /* pxSourceAddressLength can be NULL or a valid pointer. */
    if( nondet_bool() )
    {
        pxSourceAddressLength = NULL;
    }
    else
    {
        pxSourceAddressLength = ( socklen_t * ) safeMalloc( sizeof( socklen_t ) );
        if( pxSourceAddressLength != NULL )
        {
            /* Initialize to a valid size */
            *pxSourceAddressLength = sizeof( struct freertos_sockaddr );
        }
    }

    /* xFlags can be any combination of valid flags */
    /* FREERTOS_ZERO_COPY is a relevant flag here */

    __CPROVER_file_local_FreeRTOS_Sockets_c_RecvFrom_CopyPacket(
        pucEthernetBuffer,
        pvBuffer,
        uxBufferLength,
        xFlags,
        pxSourceAddress,
        pxSourceAddressLength );
}