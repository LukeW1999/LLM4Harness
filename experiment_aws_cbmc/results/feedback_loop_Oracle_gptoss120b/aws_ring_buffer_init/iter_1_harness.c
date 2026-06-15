#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_MALLOC (1024 * 1024)

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Ground‑truth preconditions */
    __CPROVER_assume(&ring_buf != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 1. Return value / error code correctness */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "aws_ring_buffer_init returns a valid status code");

    /* 2. Output buffer invariants */
    __CPROVER_assert(
        (result == AWS_OP_SUCCESS) ==>
            (ring_buf.allocation != NULL &&
             ring_buf.allocation_end == ring_buf.allocation + size &&
             ring_buf.allocator == allocator &&
             aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation &&
             aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation),
        "aws_ring_buffer_init success state invariants");

    __CPROVER_assert(
        (result == AWS_OP_ERR) ==>
            (ring_buf.allocation == NULL),
        "aws_ring_buffer_init error state invariants");

    /* 3. Structural validity predicate */
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf),
                     "ring buffer satisfies aws_ring_buffer_is_valid after init");

    /* 4. Frame conditions */
    /* The allocator pointer must remain unchanged */
    __CPROVER_assert(allocator == aws_default_allocator(),
                     "allocator pointer unchanged by aws_ring_buffer_init");

    return 0;
}
