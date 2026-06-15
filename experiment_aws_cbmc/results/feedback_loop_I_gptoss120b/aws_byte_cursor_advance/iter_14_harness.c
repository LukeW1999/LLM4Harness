#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };
    struct aws_byte_cursor old_cursor = cursor;

    uint64_t amount64 = nondet_uint64_t();
    size_t amount = (size_t)amount64;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, amount);

    /* result must be a valid operation code */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* cursor must remain a valid cursor */
    assert(aws_byte_cursor_is_valid(&cursor));

    if (result == AWS_OP_SUCCESS) {
        /* success: cursor advanced by amount */
        assert(cursor.len == old_cursor.len - amount);
        if (amount > 0) {
            assert(old_cursor.ptr != NULL);
            assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount);
        } else {
            assert(cursor.ptr == old_cursor.ptr);
        }
    } else {
        /* error: cursor unchanged */
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* frame condition: original buffer unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
}
