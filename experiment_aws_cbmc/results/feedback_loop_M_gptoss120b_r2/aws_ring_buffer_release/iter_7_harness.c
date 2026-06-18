#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    /* allocate a bounded underlying buffer on the stack */
    uint8_t allocation[MAX_BUFFER_SIZE];
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = allocation;
    ring.allocation_end = allocation + alloc_size;

    /* choose a non‑empty region inside the allocation */
    size_t region_offset = nondet_size_t();
    __CPROVER_assume(region_offset < alloc_size);
    size_t max_region_len = alloc_size - region_offset;
    size_t region_len = nondet_size_t();
    __CPROVER_assume(region_len > 0 && region_len <= max_region_len);

    /* initialise atomic head and tail */
    uint8_t *head = ring.allocation + region_offset;
    uint8_t *tail = head + region_len;
    aws_atomic_store_ptr(&ring.head, head);
    aws_atomic_store_ptr(&ring.tail, tail);

    /* assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer that will be released */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* make the byte buffer exactly the region to be released */
    buf.buffer = head;
    buf.capacity = region_len;
    buf.len = region_len; /* release the whole region */
    buf.allocator = allocator;

    /* ensure the byte buffer satisfies its own validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Post‑conditions for the released byte buffer (AWS_ZERO_STRUCT) */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 6. Post‑conditions for the ring buffer */
    void *new_head = aws_atomic_load_ptr(&ring.head);
    assert(new_head == (void *)old_ring.tail);
    assert(aws_atomic_load_ptr(&ring.tail) == aws_atomic_load_ptr(&old_ring.tail));

    /* fields that must remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring.head) == new_head);
    assert(aws_atomic_load_ptr(&ring.tail) == aws_atomic_load_ptr(&old_ring.tail));

    /* 7. Ring buffer validity invariant must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
}
