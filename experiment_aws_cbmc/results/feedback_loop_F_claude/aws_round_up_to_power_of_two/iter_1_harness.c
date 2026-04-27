/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    /* Non-deterministic input */
    size_t n;
    size_t result;

    /* Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be >= n */
        assert(result >= n);
        /* result must be the smallest power of two >= n */
        /* If result > 1, then result/2 < n */
        if (result > 1) {
            assert((result >> 1) < n);
        }
        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }
        /* result must not overflow SIZE_MAX */
        assert(result <= SIZE_MAX);
        /* result must be within valid range for power of two */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* Failure: n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* ret must be AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);
    }
}
