#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_CAPACITY 64

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic capacity bounded by MAX_CAPACITY */
    size_t capacity = nondet_uint() % (MAX_CAPACITY + 1);
    uint8_t *data = NULL;
    if (capacity > 0) {
        data = malloc(capacity);
        __CPROVER_assume(data != NULL);
    }

    /* Initialize the byte buffer */
    buf.buffer    = data;
    buf.capacity  = capacity;
    buf.len       = nondet_uint() % (capacity + 1);
    buf.allocator = alloc;

    /* Nondeterministic flag for zeroing the contents */
    bool zero_contents = nondet_bool();

    /* If zeroing is requested, the buffer must be non‑NULL and have positive capacity */
    __CPROVER_assume(!zero_contents || (buf.buffer != NULL && buf.capacity > 0));

    /* Snapshot of the original state */
    size_t old_len      = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;

    /* Call the function under verification */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postcondition assertions */
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.buffer == old_buffer);

    if (zero_contents && old_buffer != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else if (old_buffer != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == old_buffer[i]);
        }
    }

    /* Clean up */
    if (data) {
        free(data);
    }
}
