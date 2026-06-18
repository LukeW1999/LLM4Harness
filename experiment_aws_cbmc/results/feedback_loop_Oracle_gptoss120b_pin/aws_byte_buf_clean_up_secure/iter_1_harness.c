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

    /* snapshot of original state */
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_mem = NULL;
    if (old_buf.capacity > 0) {
        old_mem = (uint8_t *)malloc(old_buf.capacity);
        __CPROVER_assume(old_mem != NULL);
        memcpy(old_mem, old_buf.buffer, old_buf.capacity);
    }

    /* call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* postconditions */
    /* buffer should be cleared */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    /* allocator should remain unchanged */
    assert(buf.allocator == old_buf.allocator);
    /* the memory that was owned should be zeroed before being freed */
    if (old_buf.capacity > 0) {
        for (size_t i = 0; i < old_buf.capacity; ++i) {
            assert(old_mem[i] == 0);
        }
    }
    /* the resulting buffer should be a valid (empty) byte buffer */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean up auxiliary memory */
    free(old_mem);
    return 0;
}
