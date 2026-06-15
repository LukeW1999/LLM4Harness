#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    struct aws_byte_buf buf;
    AWS_ZERO_STRUCT(buf);
    int init_rc = aws_byte_buf_init(&buf, allocator, capacity);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);

    /* Set buf.len to a nondeterministic value no greater than capacity */
    buf.len = capacity;
    __CPROVER_assume(buf.len <= buf.capacity);

    struct aws_byte_buf buf_old = buf;
    uint8_t *buffer_old = buf.buffer;

    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    assert(aws_byte_cursor_is_valid(&cur));

    assert(cur.len == buf.len);
    if (buf.len == 0) {
        assert(cur.ptr == NULL || cur.ptr == buf.buffer);
    } else {
        assert(cur.ptr == buf.buffer);
    }

    assert(buf.len == buf_old.len);
    assert(buf.capacity == buf_old.capacity);
    assert(buf.allocator == buf_old.allocator);
    assert(buf.buffer == buffer_old);

    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == ((uint8_t *)buffer_old)[i]);
        }
    }

    if (buf.buffer != NULL) {
        allocator->deallocate(allocator, buf.buffer);
    }
}
