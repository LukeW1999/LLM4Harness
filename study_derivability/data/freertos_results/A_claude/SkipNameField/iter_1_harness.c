/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>

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

/* Maximum buffer size for bounded verification */
#ifndef MAX_BUFFER_SIZE
    #define MAX_BUFFER_SIZE    ( 256U )
#endif

/* Declaration of the function under test if not already declared */
size_t SkipNameField( const uint8_t * pucByte,
                      size_t uxLength );

void SkipNameField_harness( void )
{
    /* Nondeterministic inputs */
    size_t uxLength;
    uint8_t * pucByte;

    /* Nondeterministic length */
    uxLength = nondet_size_t();

    /* Constrain length to a reasonable bound for verification */
    __CPROVER_assume( uxLength <= MAX_BUFFER_SIZE );

    /* Allocate buffer of the given length */
    if( uxLength > 0U )
    {
        pucByte = ( uint8_t * ) malloc( uxLength );
        __CPROVER_assume( pucByte != NULL );
    }
    else
    {
        pucByte = NULL;
    }

    /* Call the function under test */
    size_t uxResult = SkipNameField( pucByte, uxLength );

    /* Postconditions:
     * 1. The result must be less than or equal to uxLength.
     *    If the name field is malformed or the buffer is too small,
     *    the function returns 0 to indicate an error.
     * 2. The result is either 0 (error) or a valid offset within the buffer.
     */

    /* Result must not exceed the input length */
    assert( uxResult <= uxLength );

    /* If uxLength is 0, the function should return 0 */
    if( uxLength == 0U )
    {
        assert( uxResult == 0U );
    }
}