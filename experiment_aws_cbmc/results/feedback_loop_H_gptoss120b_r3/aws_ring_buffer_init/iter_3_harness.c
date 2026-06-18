#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024U

void aws_ring_buffer_init_harness(void) {
    /* Declare and zero-initialize the ring buffer */
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size bounded by MAX_BUFFER_SIZE */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf.allocation != NULL);
        /* The allocator stored must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* allocation_end must point exactly size bytes after allocation */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        /* The ring buffer must satisfy its validity predicate */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure the allocation should be NULL and fields remain zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        /* The ring buffer is not valid when initialization fails */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
