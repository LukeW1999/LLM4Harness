#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    ring.allocator = aws_default_allocator();

    /* allocate a bounded underlying buffer */
    uint8_t *alloc = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(alloc != NULL);
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = alloc;
    ring.allocation_end = alloc + alloc_size;

    /* initialise atomic head and tail to the start of the allocation */
    AWS_ATOMIC_STORE_PTR(&ring.head, ring.allocation);
    AWS_ATOMIC_STORE_PTR(&ring.tail, ring.allocation);

    /* assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer that will be released */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* make the byte buffer point inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= alloc_size);
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= alloc_size - offset);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= cap);
    buf.allocator = aws_default_allocator();

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
    /* tail must be advanced to the end of the released region */
    void *new_tail = aws_atomic_load_ptr(&ring.tail);
    assert(new_tail == (void *)(old_buf.buffer + old_buf.capacity));

    /* fields that must remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring.head) == aws_atomic_load_ptr(&old_ring.head));

    /* 7. Ring buffer validity invariant must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
}
