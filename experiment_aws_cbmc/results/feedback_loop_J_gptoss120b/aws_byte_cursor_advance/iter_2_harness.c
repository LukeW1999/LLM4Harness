#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* allocator – use the default allocator as required by the harness policy */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 1. Create a bounded, valid byte buffer to back the cursor */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Initialise a cursor that points into the buffer */
    struct aws_byte_cursor cursor;
    cursor.ptr = buf.buffer;
    cursor.len = buf.len;

    /* 3. Save old state for immutability checks */
    const uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;
    struct aws_byte_buf old_buf = buf;

    /* Save a byte from the underlying buffer for later content comparison (if any) */
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_storage);
    }

    /* 4. Choose a nondeterministic amount to advance, bounded by MAX_BUFFER_SIZE */
    size_t amount;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* 5. Call the function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Advance must have succeeded: amount must be <= old_len */
        assert(amount <= old_len);
        /* Pointer and length must be updated accordingly */
        assert(cursor.ptr == old_ptr + amount);
        assert(cursor.len == old_len - amount);
        /* Remaining bytes must match the original buffer contents */
        if (cursor.len > 0) {
            assert_bytes_match(buf.buffer + amount, cursor.ptr, cursor.len);
        }
    } else {
        /* On failure the cursor must be unchanged */
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    }

    /* 7. Underlying buffer must remain unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 8. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&buf));
    /* The cursor is valid if its pointer is either NULL or points within the buffer */
    assert((cursor.ptr == NULL && cursor.len == 0) ||
           (cursor.ptr >= buf.buffer && cursor.ptr <= buf.buffer + buf.len &&
            cursor.len <= (size_t)(buf.buffer + buf.len - cursor.ptr)));
}
