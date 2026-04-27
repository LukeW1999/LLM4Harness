/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

/**
 * aws_nospec_mask(size_t index, size_t bound):
 *
 * Returns 0 if:
 *   - index >= bound
 *   - bound > (SIZE_MAX / 2)
 *   - index > (SIZE_MAX / 2)
 *
 * Returns UINTPTR_MAX otherwise.
 */

void aws_nospec_mask_harness() {
    /* Non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* Determine whether we are in the "in-range" case or "out-of-range" case */
    int index_too_big = (index > (SIZE_MAX / 2));
    int bound_too_big = (bound > (SIZE_MAX / 2));
    int index_gte_bound = (index >= bound);

    int should_be_zero = index_too_big || bound_too_big || index_gte_bound;

    if (should_be_zero) {
        /* Out of range: result must be 0 */
        assert(result == 0);
    } else {
        /* In range: result must be UINTPTR_MAX */
        assert(result == UINTPTR_MAX);
    }

    /* The result is always either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);
}
