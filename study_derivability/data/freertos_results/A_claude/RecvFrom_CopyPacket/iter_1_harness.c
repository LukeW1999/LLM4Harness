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

int32_t RecvFrom_CopyPacket( uint8_t * pucEthernetBuffer,
                              void * pvBuffer,
                              size_t uxBufferLength,
                              BaseType_t xFlags,
                              int32_t lDataLength );

/****************************************************************
* Proof harness
****************************************************************/

void RecvFrom_CopyPacket_harness( void )
{
    /* Nondeterministic inputs */
    size_t uxBufferLength;
    BaseType_t xFlags;
    int32_t lDataLength;
    int32_t result;

    /* Constrain lDataLength to be a reasonable positive value */
    __CPROVER_assume( lDataLength >= 0 );
    __CPROVER_assume( lDataLength <= ipconfigNETWORK_MTU );

    /* Constrain uxBufferLength */
    __CPROVER_assume( uxBufferLength <= ipconfigNETWORK_MTU );

    /* Allocate ethernet buffer - must be large enough for lDataLength */
    uint8_t * pucEthernetBuffer = ( uint8_t * ) malloc( lDataLength > 0 ? ( size_t ) lDataLength : 1 );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    /* Allocate destination buffer */
    void * pvBuffer = malloc( uxBufferLength > 0 ? uxBufferLength : 1 );
    __CPROVER_assume( pvBuffer != NULL );

    /* Call the function under test */
    result = RecvFrom_CopyPacket( pucEthernetBuffer,
                                  pvBuffer,
                                  uxBufferLength,
                                  xFlags,
                                  lDataLength );

    /* Postconditions */

    /* The result should not exceed lDataLength */
    assert( result <= lDataLength );

    /* If FREERTOS_ZERO_COPY flag is not set and lDataLength > 0,
     * the result should be at most uxBufferLength */
    if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
    {
        if( lDataLength > 0 )
        {
            assert( result <= ( int32_t ) uxBufferLength );
        }
    }

    /* Result should be non-negative if lDataLength is non-negative
     * (function copies min of lDataLength and uxBufferLength bytes) */
    if( lDataLength >= 0 )
    {
        assert( result >= 0 );
    }
}