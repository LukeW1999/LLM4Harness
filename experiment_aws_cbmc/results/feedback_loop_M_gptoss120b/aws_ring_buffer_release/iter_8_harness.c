#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    int init_res = aws_ring_buffer_init(&ring, allocator, size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    size_t req = nondet_size_t();
    __CPROVER_assume(req > 0 && req <= size);
    int acq_res = aws_ring_buffer_acquire(&ring, req, &buf);
    __CPROVER_assume(acq_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    size_t old_head = ring.head;
    size_t old_tail = ring.tail;
    size_t old_capacity = buf.capacity;

    aws_ring_buffer_release(&ring, &buf);

    /* After release, the buffer should be reset */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer state should remain consistent */
    assert(ring.allocator == allocator);
    assert(ring.size == size);
    assert(ring.head == old_head);
    assert(ring.tail == old_tail + old_capacity);

    assert(aws_ring_buffer_is_valid(&ring));
}
