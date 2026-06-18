#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* Zero-initialize to avoid undefined behavior from uninitialized atomics/pointers */
    struct aws_ring_buffer ring_buf = {0};

    size_t size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (size > 0) {
        if (result == AWS_OP_SUCCESS) {
            /* Success path postconditions */
            assert(aws_ring_buffer_is_valid(&ring_buf));
            assert(ring_buf.allocator == allocator);
            assert(ring_buf.allocation != NULL);
            assert(ring_buf.allocation_end == ring_buf.allocation + size);

            void * const head_ptr = aws_atomic_load_ptr(&ring_buf.head);
            void * const tail_ptr = aws_atomic_load_ptr(&ring_buf.tail);
            assert(head_ptr == (void *)ring_buf.allocation);
            assert(tail_ptr == (void *)ring_buf.allocation);
        } else {
            /* Allocation failure path */
            assert(ring_buf.allocation == NULL);
            assert(!aws_ring_buffer_is_valid(&ring_buf));
            assert(ring_buf.allocator == NULL);
        }
    } else {
        /* size == 0: early error return */
        assert(result == AWS_OP_ERR);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
