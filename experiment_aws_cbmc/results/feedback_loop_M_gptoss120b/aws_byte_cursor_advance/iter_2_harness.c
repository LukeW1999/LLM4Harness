#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_advance_harness(void) {
    /* 1. Declare and bound a source byte buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Create a cursor that points into the source buffer */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&src);
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Nondeterministic amount to advance (uninitialized => nondet) */
    size_t amount;

    /* 4. Call the function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 5. Post‑condition checks */

    /* The source buffer must remain unchanged */
    assert(src.buffer == old_cursor.ptr - old_cursor.len);
    assert(src.len == old_cursor.len + (old_cursor.ptr - src.buffer));
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* Successful advance: amount must be <= old length */
        assert(amount <= old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        /* Failure: amount > old length, cursor unchanged */
        assert(amount > old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 6. In all cases the cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
