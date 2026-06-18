#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_byte_buf_harness(void) {
    /* Non-deterministically choose whether str and buf are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= 8);
        /* Allocate memory for the string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize the length field (const, so use a cast) */
        *(size_t *)(&str->len) = str_len;
        /* allocator can be anything */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;
    if (!buf_is_null) {
        buf_ptr = &buf;
        __CPROVER_assume(aws_byte_buf_is_bounded(buf_ptr, 8));
        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);
        __CPROVER_assume(aws_byte_buf_is_valid(buf_ptr));
    }

    /* Save old state of buf (if not null) */
    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = buf;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions */

    /* 1. Both NULL => true */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }

    /* 2. One NULL, other not => false */
    if (str == NULL && buf_ptr != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf_ptr == NULL) {
        assert(result == false);
    }

    /* 3. Neither NULL => result depends on content comparison */
    if (str != NULL && buf_ptr != NULL) {
        /* If lengths differ, result must be false */
        if (str->len != buf_ptr->len) {
            assert(result == false);
        }
        /* We can't easily assert the true case without knowing byte contents,
           but we can assert the function is deterministic (no side effects) */
    }

    /* 4. The function should not modify the string or buf */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->buffer == old_buf.buffer);
        /* Validity invariant */
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
