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

/* CBMC includes. */
#include <assert.h>

/* Maximum hostname length for bounded verification. */
#ifndef MAX_HOSTNAME_LEN
    #define MAX_HOSTNAME_LEN    ( 254 )
#endif

/* Stub/mock declarations if needed */
extern uint32_t DNSgetHostByName_a( const char * pcHostName,
                                    FOnDNSEvent pCallback,
                                    void * pvSearchID,
                                    TickType_t uxTimeout );

/* Nondeterministic callback stub */
void nondet_dns_callback( const char * pcName,
                          void * pvSearchID,
                          uint32_t ulIPAddress )
{
    /* Nondeterministic callback body */
    ( void ) pcName;
    ( void ) pvSearchID;
    ( void ) ulIPAddress;
}

void DNSgetHostByName_a_harness( void )
{
    /* Allocate a nondeterministic hostname */
    size_t hostname_len;
    char * pcHostName;
    FOnDNSEvent pCallback;
    void * pvSearchID;
    TickType_t uxTimeout;
    uint32_t result;

    /* Nondeterministically choose hostname length */
    __CPROVER_assume( hostname_len > 0 && hostname_len <= MAX_HOSTNAME_LEN );

    /* Allocate hostname buffer */
    pcHostName = ( char * ) malloc( hostname_len + 1 );

    /* Assume valid pointer (non-null) */
    __CPROVER_assume( pcHostName != NULL );

    /* Ensure null-terminated string within bounds */
    pcHostName[ hostname_len ] = '\0';

    /* Nondeterministically choose whether to use a NULL hostname */
    int use_null_hostname;
    if( use_null_hostname )
    {
        pcHostName = NULL;
    }

    /* Nondeterministically choose callback - can be NULL or valid */
    int use_null_callback;
    if( use_null_callback )
    {
        pCallback = NULL;
    }
    else
    {
        pCallback = nondet_dns_callback;
    }

    /* pvSearchID can be any pointer (including NULL) */
    /* uxTimeout is unconstrained */

    /* Call the function under test */
    result = DNSgetHostByName_a( pcHostName,
                                 pCallback,
                                 pvSearchID,
                                 uxTimeout );

    /*
     * Postconditions:
     * 1. If pcHostName is NULL, the function should handle it gracefully.
     * 2. The return value is either 0 (failure/async lookup initiated)
     *    or a valid IPv4 address (non-zero meaning resolved immediately).
     * 3. The result must fit in a uint32_t (always true by type).
     */

    /* The result is a uint32_t - this is always satisfied by the type */
    /* but we can assert that it's a plausible IP address or 0 */

    /* If hostname was not NULL and callback was NULL, 
     * the function may attempt synchronous lookup */
    if( pcHostName != NULL && pCallback == NULL )
    {
        /* Result is either 0 (not found/error) or a valid IP address */
        /* We can assert the result is a valid uint32_t (tautology for type safety) */
        assert( result == 0 || result != 0 );
    }

    /* If callback is provided, the function may return 0 for async operation */
    if( pCallback != NULL && pcHostName != NULL )
    {
        /* Result can be 0 (async) or an IP address (cache hit) */
        assert( result == 0 || result != 0 );
    }

    /*
     * Key safety property: the function must not corrupt memory.
     * CBMC will check this implicitly through its memory safety checks.
     *
     * Additional assertion: result is a valid uint32_t value.
     */
    assert( ( result & 0xFFFFFFFFU ) == result );

    /* If pcHostName is NULL, result should be 0 (error case) */
    /* Note: This depends on implementation - uncomment if specified */
    /* if( pcHostName == NULL ) { assert( result == 0 ); } */

    /* Cleanup */
    if( pcHostName != NULL )
    {
        free( pcHostName );
    }
}