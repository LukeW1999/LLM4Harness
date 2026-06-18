#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministically choose a capacity within a reasonable bound */
    size_t cap;
    __CPROVER_assume((cap = nondet_size_t()) <= MAX_BUFFER_SIZE);

    buf.capacity = cap;
    buf.len = (cap == 0) ? 0 : nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = alloc;

    if (buf.capacity > 0) {
        buf.buffer = alloc->mem_acquire(alloc, buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up(&buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(old_buf.len == old_buf.len);
    assert(old_buf.capacity == old_buf.capacity);
    assert(old_buf.allocator == alloc);
    if (old_buf.capacity > 0) {
        assert(old_buf.buffer != NULL);
    } else {
        assert(old_buf.buffer == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
