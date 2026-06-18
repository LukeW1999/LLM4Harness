#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

size_t nondet_size_t(void);

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;
    /* Allocate a nondeterministic buffer within the maximum size */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
    buf.buffer = (uint8_t *)malloc(cap);
    __CPROVER_assume(buf.buffer != NULL || cap == 0);
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* Preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Snapshot original state for frame condition checks */
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_data = NULL;
    if (buf.buffer) {
        old_data = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, buf.buffer, buf.capacity);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* Postcondition 1: return value validity */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Postcondition 2: output length/capacity invariants */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* Postcondition 3: frame conditions (no modification of input buffer) */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);
    if (buf.buffer) {
        assert(memcmp(buf.buffer, old_data, buf.capacity) == 0);
    }

    /* Clean up */
    free(old_data);
    free(buf.buffer);
    return 0;
}
