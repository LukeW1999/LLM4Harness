#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    ring.allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;

    /* Initialize atomic head and tail to the start of the allocation */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring, ring.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring, ring.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer that will be released */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Constrain buf to point inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    size_t cap   = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    __CPROVER_assume(cap > 0 && cap <= ring_size - offset);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = ring.allocator;

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_tail = AWS_ATOMIC_LOAD_TAIL_PTR(&ring);
    uint8_t *old_head = AWS_ATOMIC_LOAD_HEAD_PTR(&ring);

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Post‑conditions for the released byte buffer (must be zeroed) */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 6. Post‑conditions for the ring buffer */
    /* Tail must now point to the end of the released region */
    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_buf.buffer + old_buf.capacity);
    /* Head must be unchanged */
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == old_head);
    /* All other fields must be unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* The ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring));

    return 0;
}
