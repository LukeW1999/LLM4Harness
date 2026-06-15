#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/byte_buf.h"
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_append_harness() {
    /* nondet inputs */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* bound sizes to limit CBMC unrolling */
    size_t to_capacity;
    size_t to_len;
    size_t from_len;
    __CPROVER_assume(to_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(to_len <= to_capacity);
    __CPROVER_assume(from_len <= MAX_BUFFER_SIZE);

    to.capacity = to_capacity;
    to.len = to_len;
    from.len = from_len;

    /* allocate buffers */
    if (to_capacity > 0) {
        to.buffer = malloc(to_capacity);
        __CPROVER_assume(to.buffer != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(to.buffer, to_capacity));
    } else {
        to.buffer = NULL;
    }
    if (from_len > 0) {
        from.ptr = malloc(from_len);
        __CPROVER_assume(from.ptr != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(from.ptr, from_len));
    } else {
        from.ptr = NULL;
    }

    /* ensure no overlap: from region does not intersect to buffer */
    if (to_capacity > 0 && from_len > 0) {
        __CPROVER_assume((from.ptr >= to.buffer + to_capacity) ||
                         (from.ptr + from_len <= to.buffer));
    }

    /* assume validity of structs */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* call function */
    int result = aws_byte_buf_append(&to, &from);

    /* postconditions */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* unchanged fields */
    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(from.ptr == old_from.ptr);
    assert(from.len == old_from.len);

    if (result == AWS_OP_SUCCESS) {
        /* success: len increased by from.len */
        assert(to.len == old_to.len + from.len);
        /* copied bytes match */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
        /* prefix unchanged */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }
        /* suffix unchanged if any space remains */
        if (to.capacity > old_to.len + from.len) {
            assert_bytes_match(to.buffer + old_to.len + from.len,
                               old_to.buffer + old_to.len + from.len,
                               to.capacity - (old_to.len + from.len));
        }
    } else {
        /* failure: dest too small, nothing copied */
        assert(to.len == old_to.len);
        /* buffer unchanged */
        if (to.capacity > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, to.capacity);
        }
    }
}
