#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    buf.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    /* Save a copy of the original buffer contents for later comparison */
    uint8_t *old_contents = NULL;
    if (buf.capacity > 0) {
        old_contents = malloc(buf.capacity);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, buf.buffer, buf.capacity);
    }

    bool zero_fill = __CPROVER_nondet_bool();

    int result = aws_byte_buf_reset(&buf, zero_fill);
    assert(result == AWS_OP_SUCCESS);

    /* Length must be reset to zero */
    assert(buf.len == 0);

    /* Capacity, allocator, and buffer pointer must remain unchanged */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (zero_fill && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer zeroed after reset");
        }
    } else if (buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == old_contents[i],
                             "buffer unchanged after reset when zero_fill is false");
        }
    }

    /* The buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    if (old_contents != NULL) {
        free(old_contents);
    }
}
