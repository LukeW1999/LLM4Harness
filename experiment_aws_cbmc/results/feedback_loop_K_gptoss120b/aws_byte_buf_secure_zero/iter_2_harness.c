/*=== CONTRACT ===
Preconditions:
  - __CPROVER_assume(buf != NULL);
  - __CPROVER_assume(aws_byte_buf_is_valid(buf));
  - __CPROVER_assume(buf->allocator == aws_default_allocator());

Postconditions (validity):
  - assert(aws_byte_buf_is_valid(buf));
  - assert(buf->allocator == aws_default_allocator());

Postconditions (length & capacity):
  - assert(buf->len == 0);
  - assert(buf->capacity == old_capacity);   // capacity unchanged

Postconditions (content):
  - if (old_capacity > 0) {
        // all bytes zeroed
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf->buffer[i] == 0);
        }
    } else {
        assert(buf->buffer == NULL);
    }

Postconditions (frame):
  - No memory outside of buf and its internal buffer is modified.
=== END CONTRACT ===*/

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_byte_buf buf;
    size_t capacity;

    __CPROVER_assume(capacity <= 1024);
    __CPROVER_assume(aws_byte_buf_init(&buf, alloc, capacity) == AWS_OP_SUCCESS);

    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;

    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = (uint8_t)nondet_uint();
        }
        __CPROVER_assume(buf.len <= buf.capacity);
    } else {
        buf.len = 0;
    }

    __CPROVER_assume(buf.allocator == alloc);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_secure_zero(&buf);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.allocator == alloc);
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);

    if (old_capacity > 0) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        assert(buf.buffer == NULL);
    }

    aws_byte_buf_clean_up(&buf);
}
