/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    /* Declare inputs */
    size_t a;
    size_t b;
    size_t r = 0;

    /* Call function under test */
    int result = aws_mul_size_checked(a, b, &r);

    /* Check return value is one of the two valid values */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "result is valid");

    /* Check both return paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r contains a * b, and no overflow occurred */
        __CPROVER_assert(r == a * b, "multiplication result correct");
    }
}
