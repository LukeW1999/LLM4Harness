#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic size for the ring buffer allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0);

    /* initialize the ring buffer */
    if (aws_ring_buffer_init(&ring_buf, alloc, ring_size) != 0) {
        return 0; /* abort harness if init fails */
    }

    /* nondeterministic capacity for the byte buffer */
    size_t buf_cap;
    __CPROVER_assume(buf_cap > 0);
    buf.buffer = malloc(buf_cap);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = buf_cap;
    buf.len = 0;
    buf.allocator = alloc;

    /* structural validity assumptions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save copies for frame condition checks */
    struct aws_ring_buffer ring_buf_old = ring_buf;
    struct aws_byte_buf   buf_old      = buf;

    /* call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* -------------------------------------------------------------------- */
    /* Postcondition 1: return value matches the pool‑membership predicate */
    /* -------------------------------------------------------------------- */
    bool expected = false;
    if (buf.buffer >= ring_buf.allocation &&
        (uintptr_t)buf.buffer + buf.capacity <= (uintptr_t)ring_buf.allocation_end) {
        expected = true;
    }
    assert(result == expected);

    /* -------------------------------------------------------------------- */
    /* Postcondition 2: frame condition – inputs must be unchanged */
    /* -------------------------------------------------------------------- */
    /* ring buffer fields */
    assert(ring_buf.allocation == ring_buf_old.allocation);
    assert(ring_buf.allocation_end == ring_buf_old.allocation_end);
    assert(ring_buf.allocator == ring_buf_old.allocator);
    assert(ring_buf.head.value == ring_buf_old.head.value);
    assert(ring_buf.tail.value == ring_buf_old.tail.value);

    /* byte buffer fields */
    assert(buf.buffer == buf_old.buffer);
    assert(buf.capacity == buf_old.capacity);
    assert(buf.len == buf_old.len);
    assert(buf.allocator == buf_old.allocator);

    /* clean up */
    aws_ring_buffer_clean_up(&ring_buf);
    free(buf.buffer);
    return 0;
}
