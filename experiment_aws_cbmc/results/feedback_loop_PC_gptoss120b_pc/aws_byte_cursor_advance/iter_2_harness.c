#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    /* Allocate and initialize a bounded byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Initialize a cursor that points into the buffer */
    struct aws_byte_cursor cursor;
    cursor.ptr = buf.buffer;
    cursor.len = buf.len;

    /* Save the old state of the cursor and the buffer */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_bytes;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_bytes);
    }

    /* Choose a nondeterministic amount to advance */
    size_t amount = nondet_size_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* Call the function under verification */
    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        /* Successful advance: amount must be within the original length */
        assert(amount <= old_cursor.len);
        /* The pointer and length are updated correctly */
        assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
        /* The underlying buffer contents remain unchanged */
        if (old_cursor.ptr != NULL && old_cursor.len > 0) {
            assert(*((uint8_t *)old_cursor.ptr + buf_bytes.pos) == buf_bytes.byte);
        }
    } else {
        /* Failure: cursor must be unchanged and amount exceeds original length */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
        assert(amount > old_cursor.len);
    }

    /* Frame conditions: the original buffer must remain unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(aws_byte_buf_is_valid(&buf));
}
