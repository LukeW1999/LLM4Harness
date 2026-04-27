/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_ptr_eq_harness() {
    /* aws_ptr_eq compares two pointers for equality.
     * bool aws_ptr_eq(const void *a, const void *b);
     * Returns true if a == b, false otherwise.
     * Neither pointer is dereferenced; only pointer identity is compared.
     */

    /* Non-deterministic pointer inputs */
    const void *a;
    const void *b;

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postconditions:
     * 1. If a == b, result must be true
     * 2. If a != b, result must be false
     * 3. The function is pure — no side effects
     */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, assert the direct relationship */
    assert(result == (a == b));
}
