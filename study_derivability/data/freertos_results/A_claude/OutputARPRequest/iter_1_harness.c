/*
 * FreeRTOS memory safety proofs with CBMC.
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

/* Proof helper includes. */
#include "cbmc.h"

/* The harness for OutputARPRequest. */
void OutputARPRequest_harness( void )
{
    /* Allocate a nondeterministic IP address to request ARP for. */
    uint32_t ulIPAddress = nondet_uint32_t();

    /* No constraints are strictly required on the IP address itself,
     * but we can optionally constrain it to be a valid unicast address.
     * For full coverage, leave it unconstrained. */

    /* Allocate and set up a network buffer descriptor that the function
     * internally uses via pxGetNetworkBufferWithDescriptor. This is
     * typically handled by stubs, but we ensure the global state is
     * in a valid configuration. */

    /* Ensure the IP task is initialized if needed by the internals. */
    /* Call the function under test. */
    OutputARPRequest( ulIPAddress );

    /* OutputARPRequest returns void; postconditions are checked via
     * assertions inside the function stubs and the CBMC model. */
}