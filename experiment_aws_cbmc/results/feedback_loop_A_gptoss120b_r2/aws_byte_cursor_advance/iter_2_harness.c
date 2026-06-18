#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Nondeterministic amount to advance */
    size_t amount = nondet_uint64_t();

    /* 4. Call function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Advance succeeded: amount must be <= old length */
        __CPROVER_assert(amount <= old_cursor.len, "amount <= old len on success");
        assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        /* Advance failed: cursor unchanged */
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 6. Invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&old_cursor));
}
