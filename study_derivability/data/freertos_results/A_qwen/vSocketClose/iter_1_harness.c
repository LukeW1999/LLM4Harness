#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "freertos_api.c"
#include "memory_assignments.c"

void vSocketClose_harness()
{
    Socket_t xSocket;
    FreeRTOS_Socket_t xSocketStruct;
    NetworkBufferDescriptor_t xNetworkBuffer;

    // Allocate nondeterministic inputs
    xSocket = ( Socket_t ) &xSocketStruct;
    xSocketStruct.u.xTCP.eTCPState = nondet_TCPState_t();
    xSocketStruct.u.xTCP.ulWindowSize = nondet_uint32_t();
    xSocketStruct.u.xTCP.ulMSS = nondet_uint32_t();
    xSocketStruct.u.xTCP.xTCPWindow = ( TCPWindow_t * ) &xNetworkBuffer;
    xNetworkBuffer.pucEthernetBuffer = ( uint8_t * ) nondet_void_ptr();
    xNetworkBuffer.xDataLength = nondet_uint16_t();

    // Constrain inputs
    __CPROVER_assume( xSocket != NULL );
    __CPROVER_assume( xSocketStruct.u.xTCP.xTCPWindow != NULL );
    __CPROVER_assume( xNetworkBuffer.pucEthernetBuffer != NULL );
    __CPROVER_assume( xSocketStruct.u.xTCP.eTCPState <= eCLOSE_WAIT );

    // Call the function under test
    vSocketClose( xSocket );

    // Assert postconditions
    assert( xSocketStruct.u.xTCP.eTCPState == eCLOSED );
    assert( xSocketStruct.u.xTCP.ulWindowSize == 0 );
    assert( xSocketStruct.u.xTCP.ulMSS == 0 );
    assert( xSocketStruct.u.xTCP.xTCPWindow == NULL );
}