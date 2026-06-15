#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = (size_t)nondet_uint64_t();
    __CPROVER_assume(size > 0 && size < 1024);

    if (aws_ring_buffer_init(&ring_buf, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑condition: the internal allocation should be cleared */
    assert(ring_buf.allocation == NULL);
}
