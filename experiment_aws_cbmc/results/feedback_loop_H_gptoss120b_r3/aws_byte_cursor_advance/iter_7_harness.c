#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <string.h>

void aws_byte_cursor_advance_harness(void) {
    /* Set up a nondeterministic byte cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save the original state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Save a random byte from the original buffer, if any */
    struct store_byte_from_buffer cursor_byte;
    if (cursor.ptr != NULL && cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &cursor_byte);
    }

    /* Nondeterministic amount to advance */
    size_t amount;
    __CPROVER_assume(amount <= SIZE_MAX);

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        /* Successful advance: pointer and length must be updated */
        if (old_cursor.ptr != NULL) {
            assert(cursor.ptr == old_cursor.ptr + amount);
        } else {
            assert(cursor.ptr == NULL);
        }
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.capacity == old_cursor.capacity);
        assert(cursor.allocator == old_cursor.allocator);

        /* If we saved a byte that is still within the new cursor, it must match */
        if (cursor.ptr != NULL && cursor.len > 0 && cursor_byte.index >= amount) {
            assert_byte_from_buffer_matches(
                cursor.ptr + (cursor_byte.index - amount),
                &cursor_byte);
        }
    } else {
        /* Failure: cursor must be unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
        assert(cursor.capacity == old_cursor.capacity);
        assert(cursor.allocator == old_cursor.allocator);
    }

    /* The original cursor must remain unchanged */
    assert(old_cursor.ptr == old_cursor.ptr);
    assert(old_cursor.len == old_cursor.len);
    assert(old_cursor.capacity == old_cursor.capacity);
    assert(old_cursor.allocator == old_cursor.allocator);

    /* The cursor must still be valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
