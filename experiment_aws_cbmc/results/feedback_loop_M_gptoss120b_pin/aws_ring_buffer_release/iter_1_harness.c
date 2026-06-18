#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = malloc(alloc_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + alloc_size;
    ring.allocator = aws_default_allocator();

    /* The atomic head/tail are initialized to the start of the allocation.
       The exact representation of aws_atomic_var is opaque to the harness,
       but we assume the ring buffer is valid after this initialization. */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer that will be released */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    size_t buf_cap = nondet_size_t();
    __CPROVER_assume(buf_cap <= MAX_BUFFER_SIZE);
    buf.capacity = buf_cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* Ensure the buffer lies within the ring buffer's allocation */
    __CPROVER_assume(buf.buffer >= ring.allocation);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring.allocation_end);

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Post‑conditions */

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer fields that must not change */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* The head atomic variable must remain unchanged (opaque comparison) */
    assert(ring.head == old_ring.head);
    /* The tail atomic variable is allowed to change; its new value should point to
       the end of the released buffer region, but the exact representation is
       implementation‑defined, so we do not assert its concrete value here. */

    /* 6. Ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring));
}
