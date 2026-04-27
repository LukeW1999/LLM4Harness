/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *make_arbitrary_aws_string_with_bounded_len(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);

    /* Allocate memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize the fields - cast away const for initialization */
    *(size_t *)&str->len = len;
    /* bytes are non-deterministic (already set by malloc in CBMC) */
    /* ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;

    return str;
}

void aws_string_eq_harness() {
    /* 1. Declare and set up inputs non-deterministically */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically choose whether pointers are NULL or valid */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_equals_b = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        a = make_arbitrary_aws_string_with_bounded_len(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (b_is_null) {
        b = NULL;
    } else if (a_equals_b && !a_is_null) {
        /* Allow a == b (same pointer) case */
        b = a;
    } else {
        b = make_arbitrary_aws_string_with_bounded_len(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save state before calling (strings are immutable, but save pointers) */
    const struct aws_string *old_a = a;
    const struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Assert postconditions */

    /* If a == b (same pointer), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* If either is NULL (but not both equal), result must be false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (b == NULL && a != NULL) {
        assert(result == false);
    }
    if (a == NULL && b == NULL) {
        /* a == b (both NULL), so returns true */
        assert(result == true);
    }

    /* 5. Assert that the strings themselves are unchanged (immutability) */
    /* Pointers must not change */
    assert(a == old_a);
    assert(b == old_b);

    /* String contents must not change */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(aws_string_is_valid(b));
    }

    /* 6. Consistency check: if result is true and both non-null, lengths must match */
    if (result == true && a != NULL && b != NULL) {
        assert(a->len == b->len);
    }

    /* If lengths differ and both non-null, result must be false */
    if (a != NULL && b != NULL && a->len != b->len) {
        assert(result == false);
    }
}
