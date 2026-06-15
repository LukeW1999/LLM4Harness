/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"

/* CBMC includes. */
#include "cbmc.h"

/* This pointer is maintained by the IP-task. Defined in FreeRTOS_IP.c */
extern NetworkBufferDescriptor_t * pxARPWaitingNetworkBuffer;
NetworkEndPoint_t * pxNetworkEndPoint_Temp;

/* Stub FreeRTOS_FindEndPointOnNetMask_IPv6 as its not relevant to the
 * correctness of the proof */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask_IPv6( const IPv6_Address_t * pxIPv6Address )
{
    __CPROVER_assert( pxIPv6Address != NULL, "Precondition: pxIPv6Address != NULL" );

    /* Assume at least one end-point is available */
    return pxNetworkEndPoint_Temp;
}

/* Stub FreeRTOS_FindEndPointOnNetMask as its not relevant to the
 * correctness of the proof */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress )
{
    /* Assume at least one end-point is available */
    return pxNetworkEndPoint_Temp;
}

/* Get rid of configASSERT in FreeRTOS_TCP_IP.c */
BaseType_t xIsCallingFromIPTask( void )
{
    return pdTRUE;
}

/* This is an output function and need not be tested with this proof. */
void FreeRTOS_OutputARPRequest_Multi( NetworkEndPoint_t * pxEndPoint,
                                      uint32_t ulIPAddress )
{
}

/* This function is proved elsewhere hence stubbing it out */
eResolutionLookupResult_t eARPGetCacheEntry( uint32_t * pulIPAddress,
                                             MACAddress_t * const pxMACAddress,
                                             struct xNetworkEndPoint ** ppxEndPoint )
{
    eResolutionLookupResult_t eReturn;

    __CPROVER_assert( pulIPAddress != NULL, "pulIPAddress cannot be NULL." );
    __CPROVER_assert( pxMACAddress != NULL, "pxMACAddress cannot be NULL." );
    __CPROVER_assert( ppxEndPoint != NULL, "ppxEndPoint cannot be NULL." );

    /* Return random value */
    return eReturn;
}

void ARPAgeCache_harness()
{
    TickType_t xTimeNow;

    /* Non deterministically determine the current time */
    xTimeNow = nondet_uint();

    /* Call the function to be verified */
    vARPAgeCache( xTimeNow );
}