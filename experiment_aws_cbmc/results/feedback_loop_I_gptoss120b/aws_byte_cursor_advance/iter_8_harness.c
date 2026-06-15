#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* keep a copy for frame condition */
    struct aws_byte_buf old_buf = buf;

    /* cursor */
    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };
    struct aws_byte_cursor old_cursor = cursor;

    /* nondet amount */
    size_t amount = nondet_size_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* call the function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* validity predicate */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* length invariant / postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(cursor.len == old_cursor.len - amount);
        if (amount > 0) {
            assert(old_cursor.ptr != NULL);
            assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount);
        } else {
            assert(cursor.ptr == old_cursor.ptr);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* frame condition: buffer unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
}
