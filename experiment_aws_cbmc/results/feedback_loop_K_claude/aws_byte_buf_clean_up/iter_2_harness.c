#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;

    /* Nondeterministically choose which valid state the buffer is in */
    bool has_content;
    __CPROVER_assume(has_content == 0 || has_content == 1);

    if (has_content) {
        /* Buffer with actual allocated content */
        size_t capacity;
        __CPROVER_assume(capacity > 0 && capacity <= 1024);

        size_t len;
        __CPROVER_assume(len <= capacity);

        uint8_t *buffer = malloc(capacity);
        __CPROVER_assume(buffer != NULL);

        buf.buffer    = buffer;
        buf.len       = len;
        buf.capacity  = capacity;
        buf.allocator = aws_default_allocator();
    } else {
        /* Empty/zeroed buffer */
        buf.buffer    = NULL;
        buf.len       = 0;
        buf.capacity  = 0;

        /* Allocator may or may not be set even for empty buffer */
        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);
        buf.allocator = has_allocator ? aws_default_allocator() : NULL;
    }

    /* Precondition: buf must be valid before calling clean_up */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under verification */
    aws_byte_buf_clean_up(&buf);

    /* Postconditions */
    assert(buf.allocator == NULL);
    assert(buf.buffer    == NULL);
    assert(buf.len       == 0);
    assert(buf.capacity  == 0);

    /* The resulting buffer must also satisfy the validity predicate */
    assert(aws_byte_buf_is_valid(&buf));
}
