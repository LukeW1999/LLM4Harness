#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness() {
    /* 1. Declare data structures */
    struct aws_byte_buf src;
    struct aws_byte_cursor cursor;

    /* bound src and allocate its buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* initialize cursor from src */
    cursor.buffer = src.buffer;
    cursor.len = src.len;

    /* save old state */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_src = src;

    /* nondet amount to advance */
    size_t amount = nondet_size_t();

    /* 2. Call function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 3. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* advance succeeded: cursor must have moved forward by amount */
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.buffer == (uint8_t *)old_cursor.buffer + amount);
    } else {
        /* advance failed: cursor must be unchanged */
        assert(cursor.buffer == old_cursor.buffer);
        assert(cursor.len == old_cursor.len);
    }

    /* 4. Source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 5. Global invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_cursor_is_valid(&cursor));
}
