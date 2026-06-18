#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

/* nondeterministic size generator for CBMC */
size_t nondet_size_t(void);

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministically choose a capacity within the bound */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* allocate the underlying buffer */
    buf.buffer = (uint8_t *)malloc(capacity);
    __CPROVER_assume(buf.buffer != NULL || capacity == 0);

    /* nondeterministically choose a length that does not exceed capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= capacity);

    buf.capacity = capacity;
    buf.allocator = alloc;

    /* assume the structural invariants required by the function */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot the original buffer contents for frame condition checking */
    uint8_t *old_contents = NULL;
    if (buf.buffer) {
        old_contents = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, buf.buffer, buf.capacity);
    }

    /* call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* 1. Return value / error code correctness (validity predicates) */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 2. Output buffer length/capacity invariants (length invariants) */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    assert(buf.len == cur.len);
    assert(buf.capacity == capacity);
    assert(buf.allocator == alloc);
    if (buf.buffer) {
        assert(memcmp(old_contents, buf.buffer, buf.capacity) == 0);
    }

    /* clean up */
    free(old_contents);
    free(buf.buffer);
    return 0;
}
