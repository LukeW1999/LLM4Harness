/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    /* Postconditions: result is at least each input (monotonic) */
    assert(result >= a);
    assert(result >= b);

    /* If addition does not overflow, result is exact sum */
    if (a <= SIZE_MAX - b) {
        assert(result == a + b);
    } else {
        /* Overflow case: result must be SIZE_MAX */
        assert(result == SIZE_MAX);
    }
}
