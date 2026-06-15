/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

/* Bound the length of the hostname string for CBMC analysis. */
#ifndef MAX_HOSTNAME_LEN
    #define MAX_HOSTNAME_LEN    32
#endif

void harness()
{
    /* Allocate a hostname string with bounded length. */
    size_t xNameLength;
    char * pcHostName;

    __CPROVER_assume( xNameLength > 0 && xNameLength <= MAX_HOSTNAME_LEN );

    pcHostName = malloc( xNameLength );
    __CPROVER_assume( pcHostName != NULL );

    /* Ensure the string is null-terminated within bounds. */
    pcHostName[ xNameLength - 1 ] = '\0';

    /* Call the function under test. */
    DNSgetHostByName( pcHostName );
}