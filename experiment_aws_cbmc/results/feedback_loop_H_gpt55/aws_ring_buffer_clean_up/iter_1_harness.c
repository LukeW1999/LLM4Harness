#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    if (nondet_bool()) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        __CPROVER_assume(size <= MAX_BUFFER_SIZE);

        int init_result = aws_ring_buffer_init(&ring_buf, aws_default_allocator(), size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        AWS_ZERO_STRUCT(ring_buf);
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    assert(ring_buf.allocation_end == NULL);

    assert(aws_ring_buffer_is_empty(&ring_buf));
    assert(aws_ring_buffer_is_valid(&ring_buf));

    (void)old;
}
