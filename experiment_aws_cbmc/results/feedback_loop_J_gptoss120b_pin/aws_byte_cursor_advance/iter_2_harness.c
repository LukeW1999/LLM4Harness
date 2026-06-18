#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness(void) {
    /* Set up a byte cursor with a bounded, allocated buffer */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Preserve the original state for later comparison */
    struct aws_byte_cursor old_cursor = cursor;

    /* Choose a nondeterministic amount to advance */
    size_t amount = (size_t)nondet_uint64_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* Call the function under verification */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The cursor should have moved forward by exactly `amount` bytes */
        assert(cursor.ptr == old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        /* On failure the cursor must remain unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* The cursor must still be a valid structure after the call */
    assert(aws_byte_cursor_is_valid(&cursor));
}
