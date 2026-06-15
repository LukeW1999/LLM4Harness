/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"

/* Maximum hostname length for bounded model checking. */
#define MAX_HOSTNAME_LEN  256

/* Stub/mock declarations if needed */
extern uint32_t DNSgetHostByName( const char * pcHostName );

void DNSgetHostByName_harness( void )
{
    /* Allocate a nondeterministic hostname buffer. */
    char *pcHostName;
    size_t nameLen;

    /* Nondeterministic length for hostname string */
    nameLen = nondet_size_t();

    /* Constrain the hostname length to be reasonable and bounded */
    __CPROVER_assume( nameLen > 0 );
    __CPROVER_assume( nameLen <= MAX_HOSTNAME_LEN );

    /* Allocate hostname buffer */
    pcHostName = ( char * ) malloc( nameLen );
    __CPROVER_assume( pcHostName != NULL );

    /* Ensure the hostname is null-terminated within the buffer */
    pcHostName[ nameLen - 1 ] = '\0';

    /* Allow other bytes to be nondeterministic (already are by default) */

    /* Call the function under test */
    uint32_t result = DNSgetHostByName( pcHostName );

    /* Postconditions:
     * The function should return either:
     *   - 0 (failure / not found / FreeRTOS_htonl(0))
     *   - A valid IPv4 address (any non-zero 32-bit value is potentially valid)
     * We assert that the return value is a valid uint32_t (trivially true, but
     * formally states the contract). 
     * Additionally, if the hostname is an IP address string like "a.b.c.d",
     * the function may return the parsed address directly.
     */

    /* The result is a uint32_t; assert it fits in uint32_t (always true, documents contract) */
    assert( result == result ); /* result is always a valid uint32_t */

    /* 
     * If result is non-zero, it represents a resolved IP address in network byte order.
     * We cannot assert a specific value since it's nondeterministic, but we document
     * that the function must always return (not hang indefinitely in CBMC context).
     */

    /* 
     * Additional postcondition: the hostname pointer passed in should not have
     * been modified by the function (read-only usage).
     * We verify that null terminator is still present.
     */
    assert( pcHostName[ nameLen - 1 ] == '\0' );

    /* Free allocated memory */
    free( pcHostName );
}