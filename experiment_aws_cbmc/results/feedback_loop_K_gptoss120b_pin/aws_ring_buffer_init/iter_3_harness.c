#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(alloc != NULL);

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size < (SIZE_MAX / 2));

    int result = aws_ring_buffer_init(&ring_buf, alloc, size);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "aws_ring_buffer_init returns a valid status");

    __CPROVER_assert(
        (result == AWS_OP_SUCCESS) ==>
            (ring_buf.allocation != NULL &&
             ring_buf.allocator == alloc &&
             aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation &&
             aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation &&
             ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size &&
             ring_buf.allocation_end > ring_buf.allocation),
        "postconditions when init succeeds");

    __CPROVER_assert(
        (result != AWS_OP_SUCCESS) ==>
            (ring_buf.allocation == NULL &&
             ring_buf.allocator == NULL &&
             aws_atomic_load_ptr(&ring_buf.head) == NULL &&
             aws_atomic_load_ptr(&ring_buf.tail) == NULL &&
             ring_buf.allocation_end == NULL),
        "postconditions when init fails");
}
