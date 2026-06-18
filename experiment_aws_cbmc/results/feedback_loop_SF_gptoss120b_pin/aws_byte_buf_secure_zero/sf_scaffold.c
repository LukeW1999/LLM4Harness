#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic capacity */
    size_t capacity = nondet_uint64_t();
    __CPROVER_assume(capacity <= 1024); /* optional bound */

    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }
    buf.capacity = capacity;

    /* nondeterministic length */
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    buf.allocator = alloc;

    /* precondition: valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot original state */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_contents = NULL;
    if (buf.buffer) {
        old_contents = malloc(old_capacity);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < old_capacity; ++i) {
            old_contents[i] = buf.buffer[i];
        }
    }

    /* call under test */
    aws_byte_buf_secure_zero(&buf);

    /* ASSERT_POSTCONDITIONS_HERE */

    /* cleanup */
    free(old_contents);
    free(buf.buffer);
}
