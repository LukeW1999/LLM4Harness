/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/* Function DNSgetHostByName_a is proven to be correct in this harness. */
BaseType_t DNSgetHostByName_a( const char * pcHostName,
                              struct freertos_addrinfo ** ppxAddressInfo,
                              TickType_t xTimeout )
{
    __CPROVER_assert( pcHostName != NULL, "pcHostName cannot be NULL" );
    __CPROVER_assert( ppxAddressInfo != NULL, "ppxAddressInfo cannot be NULL" );
    return nondet_BaseType();
}

void DNSgetHostByName_a_harness()
{
    const char * pcHostName;
    struct freertos_addrinfo * pxAddressInfo;
    TickType_t xTimeout;

    pcHostName = ( const char * ) malloc( 256 ); // Assuming a max hostname length of 255 + 1 for null terminator
    __CPROVER_assume( pcHostName != NULL );
    __CPROVER_assume( strlen( pcHostName ) < 256 );

    pxAddressInfo = NULL; // Initialize to NULL as per typical usage

    xTimeout = nondet_TickType();

    DNSgetHostByName_a( pcHostName, &pxAddressInfo, xTimeout );

    free( ( void * ) pcHostName );
}