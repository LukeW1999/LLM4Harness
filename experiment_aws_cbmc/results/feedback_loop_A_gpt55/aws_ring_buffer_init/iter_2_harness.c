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

    size_t size = (nondet_size_t() % MAX_BUFFER_SIZE) + 1;
    assert(size > 0);
    assert(size <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator = allocator;
    size_t old_size = size;

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    assert(allocator == old_allocator);
    assert(size == old_size);
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    bool success = result == AWS_OP_SUCCESS;
    bool error = result == AWS_OP_ERR;

    assert(!success || ring_buf.allocator == allocator);
    assert(!success || ring_buf.allocation != NULL);
    assert(!success || AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
    assert(!success || (uint8_t *)aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
    assert(!success || (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    assert(!success || ring_buf.allocation_end == ring_buf.allocation + size);
    assert(!success || aws_ring_buffer_is_valid(&ring_buf));
    assert(!success || aws_ring_buffer_is_empty(&ring_buf));

    assert(!error || ring_buf.allocator == NULL);
    assert(!error || ring_buf.allocation == NULL);
    assert(!error || (uint8_t *)aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(!error || (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    assert(!error || ring_buf.allocation_end == NULL);
    assert(!error || aws_ring_buffer_is_valid(&ring_buf));
}
