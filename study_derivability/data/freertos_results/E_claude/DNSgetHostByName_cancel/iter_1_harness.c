/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS_Callback.h"

/* CBMC includes. */
#include "cbmc.h"

void harness()
{
    size_t uxNameLength;

    /* Assume a valid, bounded length for the hostname string */
    __CPROVER_assume( uxNameLength > 0 && uxNameLength <= ipconfigDNS_CACHE_NAME_LENGTH );

    char * pcHostName = malloc( uxNameLength );
    __CPROVER_assume( pcHostName != NULL );

    /* Ensure the string is null-terminated within bounds */
    pcHostName[ uxNameLength - 1 ] = '\0';

    DNSgetHostByName_cancel( pcHostName );
}