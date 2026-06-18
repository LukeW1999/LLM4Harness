#ifdef NDEBUG
#undef NDEBUG
#endif

#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);
    assert(aws_allocator_is_valid(allocator));

    size_t size = (nondet_size_t() % MAX_BUFFER_SIZE) + 1;
    assert(size > 0);
    assert(size <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator = allocator;
    size_t old_size = size;

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    assert(allocator == old_allocator);
    assert(size == old_size);
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(aws_ring_buffer_is_empty(&ring_buf));

        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        assert(result == AWS_OP_ERR);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }
}
