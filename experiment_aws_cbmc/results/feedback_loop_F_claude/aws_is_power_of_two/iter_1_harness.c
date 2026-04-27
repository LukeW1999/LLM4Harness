/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness() {
    /* Non-deterministic input */
    size_t x = nondet_size_t();

    /* Call function under test */
    bool result = aws_is_power_of_two(x);

    /* Postconditions based on Doxygen and implementation:
     * aws_is_power_of_two returns true iff x != 0 and x has exactly one bit set.
     * i.e., x is a power of two.
     */

    /* If result is true, x must be non-zero and have exactly one bit set */
    if (result) {
        assert(x != 0);
        assert((x & (x - 1)) == 0);
    } else {
        /* If result is false, either x == 0 or x has more than one bit set */
        assert(x == 0 || (x & (x - 1)) != 0);
    }

    /* Additional cross-check: verify the return value matches the expected formula */
    bool expected = (x != 0) && ((x & (x - 1)) == 0);
    assert(result == expected);
}
