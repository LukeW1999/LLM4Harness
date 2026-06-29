#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 16

void aws_byte_buf_append_harness(void) {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Allocate backing storage for to */
    to.allocator = aws_default_allocator();
    to.capacity = nondet_size_t();
    to.len = nondet_size_t();
    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    /* Allocate backing storage for from */
    from.len = nondet_size_t();
    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Save pre-call state for frame condition checks */
    size_t old_to_capacity = to.capacity;
    size_t old_to_len = to.len;
    struct aws_allocator *old_to_allocator = to.allocator;
    uint8_t *old_to_buffer = to.buffer;

    size_t old_from_len = from.len;
    const uint8_t *old_from_ptr = from.ptr;

    /* Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* Postcondition 1: Return value correctness */
    if (to.capacity - old_to_len >= from.len) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* Should fail with AWS_ERROR_DEST_COPY_TOO_SMALL */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_DEST_COPY_TOO_SMALL);
    }

    /* Postcondition 2: Output buffer validity */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Postcondition 3: Length invariants */
    if (result == AWS_OP_SUCCESS) {
        /* On success, to->len increases by from->len */
        assert(to.len == old_to_len + old_from_len);
    } else {
        /* On failure, to->len is unchanged */
        assert(to.len == old_to_len);
    }

    /* Postcondition 4: Capacity is never modified */
    assert(to.capacity == old_to_capacity);

    /* Postcondition 5: Buffer pointer is never modified */
    assert(to.buffer == old_to_buffer);

    /* Postcondition 6: Allocator is never modified */
    assert(to.allocator == old_to_allocator);

    /* Postcondition 7: from cursor is not modified */
    assert(from.len == old_from_len);
    assert(from.ptr == old_from_ptr);

    /* Postcondition 8: to->len <= to->capacity always holds */
    assert(to.len <= to.capacity);
}

void aws_byte_buf_append_harness(void) {
    aws_byte_buf_append_harness();
    return 0;
}
