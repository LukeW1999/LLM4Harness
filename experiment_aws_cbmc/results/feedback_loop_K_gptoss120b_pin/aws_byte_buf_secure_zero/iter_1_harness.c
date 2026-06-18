/*=== Contract for aws_byte_buf_secure_zero ===
Preconditions:
  - assume(buf != NULL);
  - assume(aws_byte_buf_is_valid(buf));
Postconditions (validity):
  - assert(aws_byte_buf_is_valid(buf));
Postconditions (length):
  - assert(buf->len == 0);
Postconditions (content):
  - if (buf->capacity > 0) {
        for (size_t i = 0; i < buf->capacity; ++i) {
            assert(buf->buffer[i] == 0);
        }
    }
===*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to nondeterministically allocate and initialize a byte buffer */
static struct aws_byte_buf make_byte_buf(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024); /* limit for tractability */

    if (aws_byte_buf_init(&buf, alloc, capacity) != AWS_OP_SUCCESS) {
        /* allocation failed, set to empty buffer */
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = alloc;
    } else {
        /* nondet length within capacity */
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);

        /* fill buffer with nondet data */
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }
    return buf;
}

/* Nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf = make_byte_buf();

    /* Save original state for frame condition checks */
    struct aws_allocator *orig_alloc = buf.allocator;
    size_t orig_capacity = buf.capacity;
    uint8_t *orig_buffer = buf.buffer;

    /* Call the function under verification */
    aws_byte_buf_secure_zero(&buf);

    /* Verify postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    if (orig_capacity > 0) {
        assert(buf.buffer == orig_buffer);
        for (size_t i = 0; i < orig_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        assert(buf.buffer == NULL);
    }
    assert(buf.allocator == orig_alloc);
    assert(buf.capacity == orig_capacity);

    return 0;
}
