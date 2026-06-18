#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = (size_t)nondet_uint64_t();
    __CPROVER_assume(size > 0);

    int init_res = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    uint8_t *original_allocation = ring_buf.allocation;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(original_allocation != NULL ? ring_buf.allocation == NULL : true);
}
