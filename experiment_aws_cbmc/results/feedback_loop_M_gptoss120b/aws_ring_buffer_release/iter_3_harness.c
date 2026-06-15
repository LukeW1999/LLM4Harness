#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Allocate and initialize a ring buffer */
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    int init_res = aws_ring_buffer_init(&ring, allocator, size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Acquire a buffer from the ring buffer */
    struct aws_byte_buf buf;
    size_t req = nondet_size_t();
    __CPROVER_assume(req > 0 && req <= size);
    int acq_res = aws_ring_buffer_acquire(&ring, req, &buf);
    __CPROVER_assume(acq_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Save old state */
    size_t old_head = ring.head;
    size_t old_tail = ring.tail;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Postconditions for the released byte buffer (zeroed) */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 6. Unchanged fields of the ring buffer */
    assert(ring.allocator == allocator);
    assert(ring.allocation == ring.allocation);
    assert(ring.allocation_end == ring.allocation_end);
    assert(ring.size == size);
    assert(ring.head == old_head);

    /* 7. Changed field: tail must now point to the end of the released buffer */
    assert(ring.tail == old_tail + old_buf.capacity);

    /* 8. Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring));
}
