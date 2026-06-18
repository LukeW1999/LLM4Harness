#ifdef NDEBUG
#undef NDEBUG
#endif

#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= 16);

    assert(allocator != NULL);
    assert(size > 0);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        uint8_t *head = aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail = aws_atomic_load_ptr(&ring_buf.tail);

        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(head == ring_buf.allocation);
        assert(tail == ring_buf.allocation);
        assert(aws_ring_buffer_is_empty(&ring_buf));
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, head));
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, tail));
        assert(aws_ring_buffer_is_valid(&ring_buf));

        aws_ring_buffer_clean_up(&ring_buf);
    }

    if (result == AWS_OP_ERR) {
        uint8_t *head = aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail = aws_atomic_load_ptr(&ring_buf.tail);

        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(head == NULL);
        assert(tail == NULL);
    }
}
