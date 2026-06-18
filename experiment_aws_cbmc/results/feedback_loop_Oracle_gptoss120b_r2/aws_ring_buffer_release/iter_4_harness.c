#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* allocate a backing buffer for the ring */
    size_t alloc_sz;
    __CPROVER_assume(alloc_sz > 0 && alloc_sz <= 256);
    uint8_t *allocation = (uint8_t *)alloc->mem_allocate(alloc, alloc_sz);
    __CPROVER_assume(allocation != NULL);
    ring_buf.allocator = alloc;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + alloc_sz;
    ring_buf.size = alloc_sz;

    /* initialise atomic head and tail */
    aws_atomic_store_ptr(&ring_buf.head, allocation);
    aws_atomic_store_ptr(&ring_buf.tail, allocation);

    /* a non‑zero length of data already present in the buffer */
    size_t len;
    __CPROVER_assume(len > 0 && len <= alloc_sz);
    aws_atomic_store_ptr(&ring_buf.head, allocation + len); /* data ready for consumer */

    /* construct a byte buffer that refers to the pending data */
    buf.buffer = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    buf.capacity = len;
    buf.len = len;
    buf.allocator = alloc;

    /* pre‑conditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* function under test */
    aws_ring_buffer_release(&ring_buf, &buf);

    /* post‑conditions */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.head) ==
           (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail)); /* buffer should be empty */

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
}
