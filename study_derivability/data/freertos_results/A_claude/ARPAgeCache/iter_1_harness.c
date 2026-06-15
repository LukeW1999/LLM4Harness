/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

#include <stdlib.h>
#include <stdint.h>

/* We assume that the pxGetNetworkBufferWithDescriptor function is implemented correctly
 * and returns a valid data structure. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );
    pxNetworkBuffer->pucEthernetBuffer = malloc( xRequestedSizeBytes );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
    return pxNetworkBuffer;
}

/* Mock for vReturnEthernetFrame - required by ARPAgeCache when sending ARP requests */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer,
                           BaseType_t xReleaseAfterSend )
{
    /* Mock implementation - do nothing */
    ( void ) pxNetworkBuffer;
    ( void ) xReleaseAfterSend;
}

/* Mock for xSendEventStructToIPTask */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t * pxEvent,
                                     TickType_t xTimeout )
{
    BaseType_t xReturn;
    xReturn = nondet_BaseType_t();
    return xReturn;
}

/* Mock for vReleaseNetworkBufferAndDescriptor */
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    /* Mock implementation - do nothing */
    ( void ) pxNetworkBuffer;
}

/* External ARP cache table */
extern ARPCacheRow_t xARPCache[ ipconfigARP_CACHE_ENTRIES ];

void ARPAgeCache_harness( void )
{
    uint8_t i;

    /* Initialize the ARP cache with nondeterministic values */
    for( i = 0; i < ipconfigARP_CACHE_ENTRIES; i++ )
    {
        /* Set nondeterministic age value for each entry */
        xARPCache[ i ].ucAge = nondet_uint8_t();
        xARPCache[ i ].ulIPAddress = nondet_uint32_t();

        /* Set nondeterministic MAC address */
        xARPCache[ i ].xMACAddress.ucBytes[ 0 ] = nondet_uint8_t();
        xARPCache[ i ].xMACAddress.ucBytes[ 1 ] = nondet_uint8_t();
        xARPCache[ i ].xMACAddress.ucBytes[ 2 ] = nondet_uint8_t();
        xARPCache[ i ].xMACAddress.ucBytes[ 3 ] = nondet_uint8_t();
        xARPCache[ i ].xMACAddress.ucBytes[ 4 ] = nondet_uint8_t();
        xARPCache[ i ].xMACAddress.ucBytes[ 5 ] = nondet_uint8_t();

        /* Set valid flag nondeterministically */
        xARPCache[ i ].ucValid = nondet_uint8_t();
    }

    /* Call the function under test */
    ARPAgeCache();

    /* Postconditions:
     * After ARPAgeCache() is called, entries with ucAge > 0 should have been
     * decremented. Entries that reached age 0 should have been cleared or
     * had ARP requests sent.
     * 
     * We verify that all cache entries have valid age values (0 to ipconfigMAX_ARP_AGE)
     * and that the cache remains consistent.
     */
    for( i = 0; i < ipconfigARP_CACHE_ENTRIES; i++ )
    {
        /* The age should be a valid uint8_t value */
        assert( xARPCache[ i ].ucAge <= ipconfigMAX_ARP_AGE );
    }
}