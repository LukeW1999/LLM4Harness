/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void aws_add_size_checked_harness() {
    /* Non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r must equal a + b, and no overflow occurred */
        assert(r == a + b);
        /* Verify no overflow: if b > 0, then a <= SIZE_MAX - b */
        if (b > 0) {
            assert(a <= SIZE_MAX - b);
        }
    } else {
        /* On failure: overflow was detected */
        assert(result == AWS_OP_ERR);
        /* Overflow condition: b > 0 and a > SIZE_MAX - b */
        assert(b > 0 && a > SIZE_MAX - b);
    }

    /* The return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
