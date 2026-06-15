#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc_proof/nondet.h>
#include <cbmc_proof/proof_allocators.h>

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

uint16_t usGenerateProtocolChecksum_IPv6( const uint8_t * const pucEthernetBuffer,
                                          size_t uxBufferLength,
                                          BaseType_t xOutgoingPacket );

void usGenerateProtocolChecksum_IPv6_harness()
{
    uint8_t *pucEthernetBuffer;
    size_t uxBufferLength;
    BaseType_t xOutgoingPacket;

    // Allocate nondeterministic inputs
    pucEthernetBuffer = ( uint8_t * ) cbmc_malloc( uxBufferLength );
    uxBufferLength = nondet_size_t();
    xOutgoingPacket = nondet_BaseType_t();

    // Constrain inputs
    __CPROVER_assume( uxBufferLength > 0 );
    __CPROVER_assume( pucEthernetBuffer != NULL );

    // Call the function under test
    uint16_t checksum = usGenerateProtocolChecksum_IPv6( pucEthernetBuffer, uxBufferLength, xOutgoingPacket );

    // Assert postconditions
    assert( checksum <= 0xFFFF ); // Checksum should be a 16-bit value
}