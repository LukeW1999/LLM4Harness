/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

void DNSgetHostByName_harness()
{
    const char * pcHostName;
    struct freertos_addrinfo * pxAddressInfo;
    TickType_t xTimeout;

    pcHostName = (const char *)malloc( 100 ); /* Assuming a max hostname length of 100 for testing */
    __CPROVER_assume( pcHostName != NULL );

    /* Initialize the hostname with some non-null characters */
    __CPROVER_assume( strnlen( pcHostName, 100 ) > 0 );

    xTimeout = nondet_TickType_t();

    pxAddressInfo = DNS_gethostbyname( pcHostName, xTimeout );

    /* Check if the function returns a valid pointer or NULL */
    __CPROVER_assert( pxAddressInfo == NULL || pxAddressInfo->ai_family == FREERTOS_AF_INET, "Invalid address family" );

    free( (void *)pcHostName );
}