/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    /* Declare non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* Postconditions based on Doxygen:
     * "Adds a + b. If the result overflows returns SIZE_MAX."
     *
     * Two cases:
     * 1. No overflow: result == a + b
     * 2. Overflow: result == SIZE_MAX
     */
    if (a <= SIZE_MAX - b) {
        /* No overflow case */
        assert(result == a + b);
    } else {
        /* Overflow case */
        assert(result == SIZE_MAX);
    }

    /* The result is always either the true sum or SIZE_MAX */
    assert(result >= a || result == SIZE_MAX);
    assert(result >= b || result == SIZE_MAX);
}
