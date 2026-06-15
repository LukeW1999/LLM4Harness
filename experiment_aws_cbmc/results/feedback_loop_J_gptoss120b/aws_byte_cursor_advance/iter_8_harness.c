#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

extern int aws_byte_cursor_advance(struct aws_byte_cursor *cursor, size_t amount);

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* set up a bounded, valid byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* cursor starts at the beginning of the buffer */
    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };

    /* remember old state */
    const uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;
    struct aws_byte_buf old_buf = buf;

    /* optionally remember a byte from the buffer for later sanity checks */
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_storage);
    }

    /* nondet amount, bounded */
    size_t amount = nondet_size_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* call the function under verification */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* advance succeeded: amount must have been within the original length */
        assert(amount <= old_len);
        assert(cursor.ptr == old_ptr + amount);
        assert(cursor.len == old_len - amount);
        if (cursor.len > 0) {
            /* the remaining bytes must match the original buffer contents */
            assert_bytes_match(buf.buffer + amount, cursor.ptr, cursor.len);
        }
    } else {
        /* advance failed: cursor must be unchanged */
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    }

    /* the underlying buffer must be unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* cursor must stay within the bounds of the buffer */
    assert((cursor.ptr == NULL && cursor.len == 0) ||
           (cursor.ptr >= buf.buffer &&
            cursor.ptr <= buf.buffer + buf.len &&
            cursor.len <= (size_t)(buf.buffer + buf.len - cursor.ptr)));
}
