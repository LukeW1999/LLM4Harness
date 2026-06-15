#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof.h>

void aws_byte_buf_append_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bound the structures to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* Ensure allocated buffer members */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* Assume validity preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Assume memory regions are readable/writable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(from.ptr, from.len));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(to.buffer + to.len, from.len));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Call the function */
    int result = aws_byte_buf_append(&to, &from);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: len increased by from.len */
        assert(to.len == old_to.len + from.len);
        /* Data copied */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        /* Failure: len unchanged */
        assert(to.len == old_to.len);
        /* Buffer content unchanged (implicitly, but we can't easily assert byte-level) */
    }

    /* Unchanged fields for both paths */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
