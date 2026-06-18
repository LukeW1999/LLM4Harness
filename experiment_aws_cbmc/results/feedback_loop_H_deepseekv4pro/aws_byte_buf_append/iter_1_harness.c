#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bound the sizes to keep state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* Allocate buffer members non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* Assume the structures are valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Save old state for postcondition checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* Postconditions depending on success or failure */
    if (result == AWS_OP_SUCCESS) {
        /* On success, to->len must increase by from->len */
        assert(to.len == old_to.len + from.len);

        /* The newly appended bytes must match from->ptr */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* The existing data before the append must remain unchanged */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }
    } else {
        /* On failure, the function returns an error (AWS_OP_ERR) */
        assert(result == AWS_OP_ERR);

        /* to must be completely unchanged */
        assert(to.len == old_to.len);
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }
    }

    /* Fields that must never change, regardless of outcome */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);

    /* The source cursor must remain unchanged (it is const) */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* Both structures must still satisfy their validity invariants */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
