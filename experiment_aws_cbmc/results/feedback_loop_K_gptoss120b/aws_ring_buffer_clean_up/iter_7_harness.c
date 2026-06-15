#include <stdbool.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/allocator.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    bool do_init = nondet_bool();

    if (do_init) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        ring_buf.allocation_size = 0;
        ring_buf.allocator = allocator;
        aws_atomic_init_int(&ring_buf.head, 0);
        aws_atomic_init_int(&ring_buf.tail, 0);
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
}
