#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure(struct aws_byte_buf *buf);

void aws_byte_buf_clean_up_secure_harness(void) {
    /* Allocate a non-deterministic aws_byte_buf on the stack */
    struct aws_byte_buf buf;

    /* Non-deterministic capacity and length */
    size_t capacity;
    size_t len;
    __CPROVER_assume(capacity <= 64); /* bound for tractability */
    __CPROVER_assume(len <= capacity);

    buf.capacity = capacity;
    buf.len = len;

    /* Set up buffer memory */
    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* Use a real allocator or NULL non-deterministically */
    bool use_allocator;
    if (use_allocator) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    /* Precondition: buf must be valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    return 0;
}
