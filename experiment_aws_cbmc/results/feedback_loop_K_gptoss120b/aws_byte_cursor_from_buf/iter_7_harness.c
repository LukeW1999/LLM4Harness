#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    size_t len;
    __CPROVER_assume(len <= 1024);
    buf.len = len;
    buf.capacity = len;
    buf.allocator = allocator;

    if (len > 0) {
        buf.buffer = allocator->allocate(allocator, len);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

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
