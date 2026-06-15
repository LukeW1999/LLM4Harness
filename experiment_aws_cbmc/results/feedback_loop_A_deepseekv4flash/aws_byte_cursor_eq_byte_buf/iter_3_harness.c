#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* nondet parameters */
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buf;

    /* ensure the data structures are bounded and have allocated buffer/ptr members */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    /* ensure cursor and buffer are valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* ensure that cursor.ptr and buf.buffer are readable for their lengths */
    __CPROVER_assume(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.len));

    /* save old state */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* call the function */
    bool rv = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* compute expected result */
    bool expected;
    if (cursor.len != buf.len) {
        expected = false;
    } else {
        expected = true;
        for (size_t i = 0; i < cursor.len; ++i) {
            if (cursor.ptr[i] != buf.buffer[i]) {
                expected = false;
                break;
            }
        }
    }

    /* assert return value matches actual contents */
    assert(rv == expected);

    /* assert that both structs remain unchanged (const pointers guarantee this) */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* assert validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));
}
