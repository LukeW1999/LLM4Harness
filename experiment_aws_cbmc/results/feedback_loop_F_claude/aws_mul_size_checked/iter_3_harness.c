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

    /* Check both return paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r contains a * b, and no overflow occurred */
        __CPROVER_assert(r == a * b, "multiplication result correct");
    } else {
        /* On failure: result must be AWS_OP_ERR */
        __CPROVER_assert(result == AWS_OP_ERR, "error return value correct");
    }
}
