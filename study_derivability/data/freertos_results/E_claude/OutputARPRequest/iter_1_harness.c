/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

/* We assume that the pxGetNetworkBufferWithDescriptor function is implemented
 * correctly and returns a valid data structure. */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    __CPROVER_assume( pxNetworkBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = malloc( xRequestedSizeBytes );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
    return pxNetworkBuffer;
}

BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxDescriptor != NULL, "The network interface cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
    return 0;
}

void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
    free( pxNetworkBuffer->pucEthernetBuffer );
    free( pxNetworkBuffer );
}

void OutputARPRequest_harness()
{
    /*
     * For this proof, its assumed that the endpoints and interfaces are correctly
     * initialised and the pointers are set correctly.
     * Assumes at least one endpoint and interface is present.
     */

    pxNetworkEndPoints = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );

    /* Interface init. */
    pxNetworkEndPoints->pxNetworkInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    __CPROVER_assume( pxNetworkEndPoints->pxNetworkInterface != NULL );

    if( nondet_bool() )
    {
        pxNetworkEndPoints->pxNext = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        __CPROVER_assume( pxNetworkEndPoints->pxNext != NULL );
        pxNetworkEndPoints->pxNext->pxNext = NULL;
        pxNetworkEndPoints->pxNext->pxNetworkInterface = pxNetworkEndPoints->pxNetworkInterface;
    }
    else
    {
        pxNetworkEndPoints->pxNext = NULL;
    }

    pxNetworkEndPoints->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;

    /* Create a nondeterministic IP address to send the ARP request for. */
    uint32_t ulIPAddress;

    OutputARPRequest( ulIPAddress );
}