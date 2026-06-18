#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialize a valid byte buffer */
    ensure_byte_buf_is_valid(&buf, alloc);

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
}
