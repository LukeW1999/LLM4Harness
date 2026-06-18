#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic capacity bounded by MAX_BUFFER_SIZE */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* allocate buffer memory */
    uint8_t *mem = NULL;
    if (capacity > 0) {
        mem = (uint8_t *)malloc(capacity);
        __CPROVER_assume(mem != NULL);
        /* nondeterministically initialize the memory */
        for (size_t i = 0; i < capacity; ++i) {
            mem[i] = (uint8_t)__CPROVER_nondet_uint();
        }
    }

    /* construct aws_byte_buf */
    struct aws_byte_buf buf;
    buf.allocator = alloc;
    buf.buffer    = mem;
    buf.capacity  = capacity;

    /* nondeterministic length respecting capacity */
    size_t len;
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* assume structural preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot of original state (only allocator needed for later check) */
    struct aws_byte_buf old_buf = buf;

    /* call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    /* allocator should remain unchanged or be NULL (implementation dependent) */
    assert(buf.allocator == old_buf.allocator || buf.allocator == NULL);
    /* the resulting buffer should be a valid (empty) byte buffer */
    assert(aws_byte_buf_is_valid(&buf));
}
