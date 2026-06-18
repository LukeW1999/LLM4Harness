#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_append_harness(void) {
    /*--- Setup ---*/
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* allocate and initialize `to` */
    to.capacity = nondet_size_t();
    __CPROVER_assume(to.capacity <= MAX_BUFFER_SIZE);
    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= to.capacity);
    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }
    to.allocator = aws_default_allocator();

    /* allocate and initialize `from` */
    from.len = nondet_size_t();
    __CPROVER_assume(from.len <= MAX_BUFFER_SIZE);
    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /*--- Ground‑truth preconditions ---*/
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /*--- Preserve original state for frame conditions ---*/
    size_t old_len = to.len;
    size_t old_capacity = to.capacity;
    uint8_t *old_buf = NULL;
    if (to.capacity > 0) {
        old_buf = malloc(to.capacity);
        __CPROVER_assume(old_buf != NULL);
        memcpy(old_buf, to.buffer, to.capacity);
    }

    uint8_t *old_from_buf = NULL;
    if (from.len > 0) {
        old_from_buf = malloc(from.len);
        __CPROVER_assume(old_from_buf != NULL);
        memcpy(old_from_buf, from.ptr, from.len);
    }

    /*--- Call function under test ---*/
    int rc = aws_byte_buf_append(&to, &from);

    /*--- Postconditions ---*/
    if (rc == AWS_OP_SUCCESS) {
        assert(rc == AWS_OP_SUCCESS);
    } else {
        assert(rc == aws_raise_error(AWS_ERROR_DEST_COPY_TOO_SMALL));
    }

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    assert(to.capacity == old_capacity);
    if (rc == AWS_OP_SUCCESS) {
        assert(to.len == old_len + from.len);
    } else {
        assert(to.len == old_len);
    }

    if (to.capacity > 0) {
        /* bytes before the original length must be unchanged */
        for (size_t i = 0; i < old_len; ++i) {
            assert(to.buffer[i] == old_buf[i]);
        }

        if (rc == AWS_OP_SUCCESS && from.len > 0) {
            /* bytes written by the function must match source */
            for (size_t i = 0; i < from.len; ++i) {
                assert(to.buffer[old_len + i] == from.ptr[i]);
            }
        } else {
            /* region that could have been written must remain unchanged */
            size_t max_i = (old_len < to.capacity) ? to.capacity - old_len : 0;
            for (size_t i = 0; i < max_i; ++i) {
                assert(to.buffer[old_len + i] == old_buf[old_len + i]);
            }
        }

        /* bytes after the region possibly written must be unchanged */
        size_t after = (rc == AWS_OP_SUCCESS) ? (old_len + from.len) : old_len;
        for (size_t i = after; i < to.capacity; ++i) {
            assert(to.buffer[i] == old_buf[i]);
        }
    }

    if (from.len > 0) {
        for (size_t i = 0; i < from.len; ++i) {
            assert(from.ptr[i] == old_from_buf[i]);
        }
    }

    /* allocator must remain unchanged */
    assert(to.allocator == aws_default_allocator());

    return;
}
