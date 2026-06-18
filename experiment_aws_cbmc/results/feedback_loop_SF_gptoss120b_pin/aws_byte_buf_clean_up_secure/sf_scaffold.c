#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* nondeterministic initialization */
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity >= buf.len);

    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    } else {
        buf.buffer = NULL;
    }

    if (nondet_bool()) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    /* precondition: buffer must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot of input state */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_contents = NULL;
    if (old_buffer != NULL) {
        old_contents = malloc(old_capacity);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, old_buffer, old_capacity);
    }

    /* call the function under verification */
    aws_byte_buf_clean_up_secure(&buf);

    /* ASSERT_POSTCONDITIONS_HERE */
}
