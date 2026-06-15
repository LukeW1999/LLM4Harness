#include <stdint.h>
#include <stddef.h>
#include <cbmc_proof/nondet.h>
#include <cbmc_proof/proof_allocators.h>
#include <FreeRTOS.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

void pxDuplicateNetworkBufferWithDescriptor_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer = (NetworkBufferDescriptor_t *)can_fail_malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)can_fail_malloc(ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    pxNetworkBuffer->xDataLength = nondet_uint16_t();
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    pxNetworkBuffer->xIPAddress.ulIP_IPv4 = nondet_uint32_t();
    pxNetworkBuffer->usPort = nondet_uint16_t();
    pxNetworkBuffer->usBoundPort = nondet_uint16_t();
    pxNetworkBuffer->ucFlags = nondet_uint8_t();

    NetworkBufferDescriptor_t *pxNewBuffer = pxDuplicateNetworkBufferWithDescriptor(pxNetworkBuffer, pxNetworkBuffer->xDataLength);

    if (pxNewBuffer != NULL)
    {
        assert(pxNewBuffer->xDataLength == pxNetworkBuffer->xDataLength);
        assert(pxNewBuffer->xIPAddress.ulIP_IPv4 == pxNetworkBuffer->xIPAddress.ulIP_IPv4);
        assert(pxNewBuffer->usPort == pxNetworkBuffer->usPort);
        assert(pxNewBuffer->usBoundPort == pxNetworkBuffer->usBoundPort);
        assert(pxNewBuffer->ucFlags == pxNetworkBuffer->ucFlags);
        assert(pxNewBuffer->pucEthernetBuffer != NULL);
        assert(__CPROVER_w_ok(pxNewBuffer->pucEthernetBuffer, pxNewBuffer->xDataLength));
        assert(__CPROVER_r_ok(pxNewBuffer->pucEthernetBuffer, pxNewBuffer->xDataLength));

        for (size_t i = 0; i < pxNetworkBuffer->xDataLength; i++)
        {
            assert(pxNewBuffer->pucEthernetBuffer[i] == pxNetworkBuffer->pucEthernetBuffer[i]);
        }
    }
}