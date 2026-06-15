/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"

/* Maximum sizes for bounded verification */
#define MAX_PACKET_SIZE  ( 1024 )
#define MAX_NAME_SIZE    ( 256 )

/* Declaration of the function under test */
size_t ReadNameField( const uint8_t * pucByte,
                      size_t uxSourceLen,
                      char * pcName,
                      size_t uxDestLen );

void ReadNameField_harness( void )
{
    /* Nondeterministic sizes */
    size_t uxSourceLen;
    size_t uxDestLen;

    /* Choose bounded sizes for source and destination buffers */
    __CPROVER_assume( uxSourceLen <= MAX_PACKET_SIZE );
    __CPROVER_assume( uxDestLen <= MAX_NAME_SIZE );

    /* Allocate source buffer */
    uint8_t * pucByte = NULL;

    if( uxSourceLen > 0 )
    {
        pucByte = ( uint8_t * ) malloc( uxSourceLen );
        __CPROVER_assume( pucByte != NULL );
    }

    /* Allocate destination name buffer */
    char * pcName = NULL;

    if( uxDestLen > 0 )
    {
        pcName = ( char * ) malloc( uxDestLen );
        __CPROVER_assume( pcName != NULL );
    }

    /* Call the function under test */
    size_t uxResult = ReadNameField( pucByte, uxSourceLen, pcName, uxDestLen );

    /* Postconditions:
     * 1. The returned value must be <= uxSourceLen
     *    (cannot have consumed more bytes than were available)
     * 2. If uxSourceLen == 0 or pucByte == NULL, result should be 0
     * 3. The result is a valid size_t (non-negative by type)
     */

    /* The number of bytes consumed cannot exceed the source length */
    assert( uxResult <= uxSourceLen );

    /* If source length is 0, function must return 0 (nothing to parse) */
    if( uxSourceLen == 0 )
    {
        assert( uxResult == 0 );
    }

    /* If destination length is 0 and source length > 0, result should be 0
     * because there's nowhere to write the name */
    if( uxDestLen == 0 && uxSourceLen > 0 )
    {
        assert( uxResult == 0 );
    }

    /* If pcName is NULL and uxDestLen > 0, result should be 0 */
    if( pcName == NULL && uxDestLen > 0 )
    {
        assert( uxResult == 0 );
    }

    /* If result > 0 and pcName was provided with uxDestLen > 0,
     * ensure the name buffer is null-terminated within bounds */
    if( uxResult > 0 && pcName != NULL && uxDestLen > 0 )
    {
        /* The written name must be null-terminated within the destination buffer */
        int null_found = 0;
        for( size_t i = 0; i < uxDestLen; i++ )
        {
            if( pcName[ i ] == '\0' )
            {
                null_found = 1;
                break;
            }
        }
        assert( null_found == 1 );
    }
}