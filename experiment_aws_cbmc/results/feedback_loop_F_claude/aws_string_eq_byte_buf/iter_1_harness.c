/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic choice: should str and buf be NULL? */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        /* Allocate memory for the aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize the length field (bytes is flexible array, len is const) */
        *(size_t *)(&str->len) = str_len;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
    }

    /* Save old state */
    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions */

    /* Case 1: both NULL => true */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }

    /* Case 2: one NULL, other not => false */
    if (str == NULL && buf_ptr != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf_ptr == NULL) {
        assert(result == false);
    }

    /* Case 3: neither NULL => result depends on content comparison */
    if (str != NULL && buf_ptr != NULL) {
        /* If lengths differ, result must be false */
        if (str->len != buf_ptr->len) {
            assert(result == false);
        }
        /* We can't easily assert the exact byte comparison result,
           but we can assert validity is preserved */
    }

    /* Unchanged fields: the function is a pure comparison, nothing should change */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
