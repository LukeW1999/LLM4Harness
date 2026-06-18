#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t buf_size = nondet_size_t();
    __CPROVER_assume(buf_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_size > 0);

    /* Initialize the ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, buf_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Set head and tail to nondeterministic but valid values */
    uint64_t head_val = nondet_uint64_t();
    __CPROVER_assume(head_val <= ring_buf.size);
    uint64_t tail_val = nondet_uint64_t();
    __CPROVER_assume(tail_val <= ring_buf.size);

    aws_atomic_init_uint64(&ring_buf.head, head_val);
    aws_atomic_init_uint64(&ring_buf.tail, tail_val);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Verify post‑conditions */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_uint64(&ring_buf.head) == 0);
    assert(aws_atomic_load_uint64(&ring_buf.tail) == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
