/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Stream_Buffer.h"
/* CBMC includes. */
#include "cbmc.h"
#include "../../utility/memory_assignments.c"

/* This proof assumes pxTCPSocketLookup and pxGetNetworkBufferWithDescriptor
 * are implemented correctly.
 *
 * It also assumes prvSingleStepTCPHeaderOptions, prvCheckOptions, prvTCPPrepareSend,
 * prvTCPHandleState, prvTCPSendRepeated are implemented correctly.
 */

/* Abstraction of pxTCPSocketLookup */
FreeRTOS_Socket_t * pxTCPSocketLookup( uint32_t ulLocalIP,
                                        UBaseType_t uxLocalPort,
                                        uint32_t ulRemoteIP,
                                        UBaseType_t uxRemotePort )
{
    return ensure_FreeRTOS_Socket_t_is_allocated();
}

/* Abstraction of pxGetNetworkBufferWithDescriptor */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                               TickType_t xBlockTimeTicks )
{
    return ensure_NetworkBufferDescriptor_is_allocated();
}

void ProcessReceivedTCPPacket_harness( void )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xPacketSize;
    BaseType_t xResult;

    /* Allocate a network buffer descriptor nondeterministically */
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    /* We need a valid network buffer descriptor */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate payload buffer for the network buffer */
    /* Minimum size must include Ethernet header + IP header + TCP header */
    size_t bufferSize;
    __CPROVER_assume( bufferSize >= sizeof( EthernetHeader_t ) +
                                    sizeof( IPHeader_t ) +
                                    sizeof( TCPHeader_t ) );
    __CPROVER_assume( bufferSize <= ipconfigNETWORK_MTU + sizeof( EthernetHeader_t ) );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( bufferSize );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = bufferSize;

    /* Set nondeterministic interface and endpoint */
    pxNetworkBuffer->pxInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );

    /* Call the function under test */
    xResult = ProcessReceivedTCPPacket( pxNetworkBuffer );

    /* Postconditions:
     * The return value must be pdTRUE or pdFALSE */
    assert( xResult == pdTRUE || xResult == pdFALSE );
}