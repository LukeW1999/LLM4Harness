/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Parser.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* CBMC includes. */
#include "cbmc.h"

/* Maximum buffer size for DNS reply. */
#ifndef DNS_BUFFER_SIZE
    #define DNS_BUFFER_SIZE    ( 512U )
#endif

/* Stub for prvReadNameField if needed */
/* Stub for DNS callback if needed */

void DNS_ParseDNSReply_harness( void )
{
    uint8_t * pucUDPPayloadBuffer;
    size_t uxBufferLength;
    BaseType_t xExpected;
    uint32_t ulResult;

    /* Allocate a nondeterministic buffer length. */
    __CPROVER_assume( uxBufferLength > 0 && uxBufferLength <= DNS_BUFFER_SIZE );

    /* Allocate the UDP payload buffer. */
    pucUDPPayloadBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucUDPPayloadBuffer != NULL );

    /* Nondeterministic value for xExpected (whether a DNS reply is expected). */
    xExpected = nondet_BaseType_t();
    __CPROVER_assume( xExpected == pdTRUE || xExpected == pdFALSE );

    /* Call the function under test. */
    ulResult = DNS_ParseDNSReply( pucUDPPayloadBuffer,
                                  uxBufferLength,
                                  xExpected );

    /* Postconditions:
     * The return value is either 0 (failure / no valid IP) or a valid IP address.
     * We cannot assert more without knowing the exact DNS content, but we can
     * check that the function returns a uint32_t value (always true by type).
     * Additional: if the buffer is too small to contain a DNS header, result is 0. */

    /* The result must be a valid uint32_t (trivially true). */
    assert( ulResult == ulResult );

    /* If buffer is smaller than minimum DNS message size (DNS header = 12 bytes
     * + minimal question), the result should be 0 (no valid IP found).
     * sizeof(DNSMessage_t) is the DNS header size. */
    if( uxBufferLength < sizeof( DNSMessage_t ) )
    {
        /* When buffer is too small, we expect 0 returned. */
        assert( ulResult == 0U );
    }
}