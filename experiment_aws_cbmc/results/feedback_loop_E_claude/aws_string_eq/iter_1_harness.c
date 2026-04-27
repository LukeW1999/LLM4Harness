/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_harness() {
    /* Declare two aws_string pointers non-deterministically (may be NULL) */
    struct aws_string *a = ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);
    struct aws_string *b = ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);

    /* If non-NULL, assume they are valid */
    if (a != NULL) {
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (b != NULL) {
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save state before call */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */

    /* If a == b (same pointer), result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* If either is NULL (and they're not the same pointer), result must be false */
    if (a == NULL && b != b) { /* unreachable but for clarity */ }
    if (a != b && (a == NULL || b == NULL)) {
        assert(result == false);
    }

    /* If both are non-NULL and different pointers, result depends on content */
    if (a != NULL && b != NULL && a != b) {
        /* If lengths differ, result must be false */
        if (a->len != b->len) {
            assert(result == false);
        }
    }

    /* Strings must remain valid and unchanged after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
        assert(b->len == old_b_len);
    }

    /* Result is a bool */
    assert(result == true || result == false);
}
```

Wait, I need to use the correct helper function. Let me check what's available and write a proper harness.

```c
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_harness() {
    /* Declare two aws_string structs non-deterministically */
    struct aws_string *a = nondet_bool() ? NULL : ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);
    struct aws_string *b = nondet_bool() ? NULL : ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);

    /* If non-NULL, assume they are valid */
    if (a != NULL) {
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (b != NULL) {
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save lengths before call */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: if a == b (same pointer), must return true */
    if (a == b) {
        assert(result == true);
    }

    /* Postcondition: if one is NULL and they differ, must return false */
    if (a != b && (a == NULL || b == NULL)) {
        assert(result == false);
    }

    /* Postcondition: if both non-NULL and lengths differ, must return false */
    if (a != NULL && b != NULL && a->len != b->len) {
        assert(result == false);
    }

    /* Strings must remain valid and unchanged after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
        assert(b->len == old_b_len);
    }

    /* Result is a bool */
    assert(result == true || result == false);
}
