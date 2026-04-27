/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    /* Declare non-deterministic inputs */
    size_t a;
    size_t b;

    /* Call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postconditions based on Doxygen:
     * "Multiplies a * b. If the result overflows, returns SIZE_MAX."
     */

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a <= SIZE_MAX / b) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }
}
