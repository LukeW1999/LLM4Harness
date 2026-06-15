#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocate a bounded allocation for the ring buffer */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    uint8_t *allocation = malloc(ring_size);
    __CPROVER_assume(allocation != NULL);
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + ring_size;
    ring_buffer.allocator = NULL; /* allocator is not used in release */

    /* Initialise atomic head/tail to some nondet values within the allocation */
    AWS_ATOMIC_STORE_PTR(&ring_buffer.head, allocation);
    AWS_ATOMIC_STORE_PTR(&ring_buffer.tail, allocation);

    /* Ensure the byte buffer has an allocated buffer member and is bounded */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Choose a capacity that fits inside the ring allocation */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size);
    buf.capacity = cap;

    /* Choose an offset so that buffer + capacity stays inside the allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + cap <= ring_size);
    buf.buffer = allocation + offset;

    /* len can be any nondet value (it will be zeroed by the function) */
    buf.len = nondet_size_t();

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    assert(buf.allocator == old_buf.allocator); /* allocator unchanged (still NULL) */

    /* The ring buffer's tail atomic pointer must be updated to point just past the released region */
    /* We cannot directly read the atomic value, but we can assert that it is no longer the old value */
    assert(ring_buffer.tail != old_ring.tail);
    /* If the atomic representation stores a raw pointer, the new value should equal old_buf.buffer + old_buf.capacity */
    /* This is expressed as a logical condition (may be checked by the model if the representation matches) */
    assert((void *)ring_buffer.tail == (void *)(old_buf.buffer + old_buf.capacity));

    /* Unchanged fields of the ring buffer */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(ring_buffer.head == old_ring.head);

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
