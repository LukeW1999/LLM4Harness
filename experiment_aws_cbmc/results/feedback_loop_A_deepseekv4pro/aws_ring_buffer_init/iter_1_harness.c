#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    size_t size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bound the allocation size to keep state space manageable */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* The ring buffer struct itself must be accessible for writes */
    __CPROVER_assume(__CPROVER_w_ok(&ring_buf, sizeof(struct aws_ring_buffer)));

    /* 2. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success path postconditions */
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail atomic pointers are initialized to the start of the allocation */
        void * const head_ptr = aws_atomic_load_ptr(&ring_buf.head);
        void * const tail_ptr = aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == (void *)ring_buf.allocation);
        assert(tail_ptr == (void *)ring_buf.allocation);
    } else {
        /* Failure path: allocation must be NULL (malloc failed) */
        assert(ring_buf.allocation == NULL);
        /* The ring buffer is not valid */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
        /* The whole struct was zeroed before the failed allocation,
         * so allocator is NULL. Other fields are zero as well. */
        assert(ring_buf.allocator == NULL);
    }

    /* The function is an initializer; there are no "unchanged" fields
     * to compare with the old state because it overwrites all fields.
     * The validity invariant is covered above conditionally. */
}
