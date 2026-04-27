/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministic c_str input */
    /* Either NULL or a valid null-terminated string */
    const char *c_str;

    /* Non-deterministically choose between NULL and a valid string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Create a bounded null-terminated string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *str = malloc(len + 1);
        __CPROVER_assume(str != NULL);
        str[len] = '\0';
        c_str = str;
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions based on Doxygen and implementation */

    /* The function creates a byte buf from a c_str */
    /* allocator is always NULL (no allocation is done) */
    assert(buf.allocator == NULL);

    if (c_str == NULL) {
        /* NULL input: len = 0, capacity = 0, buffer = NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        /* Non-NULL input: len = strlen(c_str), capacity = len, buffer = (uint8_t*)c_str */
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        if (expected_len == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer == (uint8_t *)c_str);
        }
    }

    /* len == capacity always */
    assert(buf.len == buf.capacity);

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
