/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

/* We assume that the pxGetNetworkBufferWithDescriptor function is implemented correctly
 * and returns a valid data structure. */
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
}

void ARPProcessPacket_harness()
{
    /*
     * For this proof, its assumed that the endpoints and interfaces are correctly
     * initialised and the pointers are set correctly.
     */

    /* Set up the network endpoints. */
    pxNetworkEndPoints = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );

    /* Interface init. */
    pxNetworkEndPoints->pxNetworkInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    __CPROVER_assume( pxNetworkEndPoints->pxNetworkInterface != NULL );

    pxNetworkEndPoints->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;

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

    /* Allocate the network buffer descriptor for the ARP packet. */
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate an Ethernet buffer large enough to hold the ARP packet. */
    size_t uxBufferLength = sizeof( ARPPacket_t ) + ipBUFFER_PADDING;
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = uxBufferLength;

    /* Associate the endpoint with the network buffer. */
    pxNetworkBuffer->pxEndPoint = pxNetworkEndPoints;

    /* Associate the network interface with the network buffer. */
    pxNetworkBuffer->pxInterface = pxNetworkEndPoints->pxNetworkInterface;

    /* Cast the buffer to an ARP packet and let CBMC fill it non-deterministically. */
    ARPPacket_t * pxARPFrame = ( ARPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

    /* Call the function under test. */
    eARPProcessPacket( pxNetworkBuffer );
}