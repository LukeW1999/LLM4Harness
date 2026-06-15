/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"
/* CBMC includes. */
#include "cbmc.h"

/* Global variables. */
BaseType_t xIsIPv6;

/* Abstraction of xIsIPInARPCache. */
BaseType_t xIsIPInARPCache( uint32_t ulAddressToLookup )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Abstraction of xIsIPv6Loopback. */
BaseType_t xIsIPv6Loopback( const IPv6_Address_t * pxAddress )
{
    BaseType_t xReturn;
    return xReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnNetMask. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress,
                                                     uint32_t ulCaller )
{
    NetworkEndPoint_t * pxReturn;
    return pxReturn;
}

/* Abstraction of FreeRTOS_FindEndPointOnNetMask_IPv6. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask_IPv6( const IPv6_Address_t * pxIPv6Address )
{
    NetworkEndPoint_t * pxReturn;
    return pxReturn;
}

void xCheckRequiresARPResolution_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    BaseType_t xResult;

    /* Allocate a network buffer descriptor nondeterministically. */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate the ethernet buffer for the network buffer. */
    size_t xBufferSize;
    __CPROVER_assume( xBufferSize >= sizeof( EthernetHeader_t ) + sizeof( IPHeader_t ) );
    __CPROVER_assume( xBufferSize <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xBufferSize );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
    pxNetworkBuffer->xDataLength = xBufferSize;

    /* Set up an endpoint for the network buffer nondeterministically. */
    NetworkEndPoint_t * pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    pxNetworkBuffer->pxEndPoint = pxEndPoint;

    /* Set the global IPv6 flag nondeterministically. */
    /* xIsIPv6 is set nondeterministically by CBMC. */

    /* Call the function under test. */
    xResult = xCheckRequiresARPResolution( pxNetworkBuffer );

    /* Assert postconditions: result must be pdTRUE or pdFALSE. */
    assert( ( xResult == pdTRUE ) || ( xResult == pdFALSE ) );
}