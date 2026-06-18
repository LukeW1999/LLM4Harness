#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_ring_buffer_is_empty(&ring_buf));
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, aws_atomic_load_ptr(&ring_buf.head)));
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, aws_atomic_load_ptr(&ring_buf.tail)));
    } else {
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_ring_buffer_is_empty(&ring_buf));
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
