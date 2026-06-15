#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers for CBMC */
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    buf.allocator = aws_default_allocator();

    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* Fill buffer with nondet data */
        for (size_t i = 0; i < capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    } else {
        buf.buffer = NULL;
    }

    /* Snapshot of the original buffer contents */
    uint8_t *buf_snapshot = NULL;
    if (capacity > 0) {
        buf_snapshot = malloc(capacity);
        __CPROVER_assume(buf_snapshot != NULL);
        for (size_t i = 0; i < capacity; ++i) {
            buf_snapshot[i] = buf.buffer[i];
        }
    }

    /* Structural preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* Post‑condition 1: returned cursor is valid and matches the buffer */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* Post‑condition 2: the input buffer itself is unchanged */
    assert(buf.len == len);
    assert(buf.capacity == capacity);
    assert(buf.allocator == aws_default_allocator());

    /* Post‑condition 3: memory pointed to by the buffer is unchanged */
    if (capacity > 0) {
        for (size_t i = 0; i < capacity; ++i) {
            assert(buf.buffer[i] == buf_snapshot[i]);
        }
    }

    /* Clean up */
    if (buf.buffer) {
        free(buf.buffer);
    }
    if (buf_snapshot) {
        free(buf_snapshot);
    }

    return 0;
}
