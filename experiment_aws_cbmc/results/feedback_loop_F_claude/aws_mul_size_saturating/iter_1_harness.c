/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    /* Declare non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postconditions based on Doxygen:
     * "Multiplies a * b. If the result overflows, returns SIZE_MAX."
     *
     * Two cases:
     * 1. No overflow: result == a * b
     * 2. Overflow: result == SIZE_MAX
     */

    /* Check if overflow would occur using 64-bit arithmetic (or check via division) */
    /* We verify the contract: either result is the true product or SIZE_MAX on overflow */

    if (a == 0 || b == 0) {
        /* No overflow possible when either operand is 0 */
        assert(result == 0);
    } else if (a <= SIZE_MAX / b) {
        /* No overflow: a * b fits in size_t */
        assert(result == a * b);
    } else {
        /* Overflow: must return SIZE_MAX */
        assert(result == SIZE_MAX);
    }

    /* General invariant: result is always either the true product or SIZE_MAX */
    assert(result == SIZE_MAX || result == a * b);
}
