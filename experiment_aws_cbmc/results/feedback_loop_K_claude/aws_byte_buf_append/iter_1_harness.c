#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound for CBMC tractability */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_append_harness(void) {
    /* ---- Set up `to` (aws_byte_buf) ---- */
    struct aws_byte_buf to;

    /* Nondeterministic capacity and len, bounded for CBMC */
    size_t to_capacity;
    size_t to_len;
    __CPROVER_assume(to_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(to_len <= to_capacity);

    to.capacity = to_capacity;
    to.len = to_len;
    to.allocator = aws_default_allocator();

    /* Allocate buffer according to validity predicate */
    if (to_capacity > 0) {
        to.buffer = malloc(to_capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    /* Confirm to is valid before call */
    assert(aws_byte_buf_is_valid(&to));

    /* ---- Set up `from` (aws_byte_cursor) ---- */
    struct aws_byte_cursor from;

    size_t from_len;
    __CPROVER_assume(from_len <= MAX_BUFFER_SIZE);
    from.len = from_len;

    if (from_len > 0) {
        uint8_t *from_buf = malloc(from_len);
        __CPROVER_assume(from_buf != NULL);
        from.ptr = from_buf;
    } else {
        from.ptr = NULL;
    }

    /* Confirm from is valid before call */
    assert(aws_byte_cursor_is_valid(&from));

    /* ---- Save pre-call state ---- */
    size_t old_to_len      = to.len;
    size_t old_to_capacity = to.capacity;
    uint8_t *old_to_buffer = to.buffer;
    struct aws_allocator *old_to_allocator = to.allocator;
    size_t old_from_len    = from.len;
    const uint8_t *old_from_ptr = from.ptr;

    /* ---- Call the function under test ---- */
    int result = aws_byte_buf_append(&to, &from);

    /* ---- Postconditions (validity) ---- */

    /* Return value must be 0 (success) or non-zero (error) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* to and from validity preserved */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* ---- Postconditions (length / correctness) ---- */

    bool had_capacity = (old_to_capacity - old_to_len) >= old_from_len;

    if (had_capacity) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
        /* Length updated correctly */
        assert(to.len == old_to_len + old_from_len);
    } else {
        /* Should fail */
        assert(result == AWS_OP_ERR);
        /* Length unchanged */
        assert(to.len == old_to_len);
    }

    /* ---- Postconditions (frame) ---- */

    /* Capacity, buffer pointer, and allocator must not change */
    assert(to.capacity  == old_to_capacity);
    assert(to.buffer    == old_to_buffer);
    assert(to.allocator == old_to_allocator);

    /* from fields must not change */
    assert(from.len == old_from_len);
    assert(from.ptr == old_from_ptr);

    return 0;
}
