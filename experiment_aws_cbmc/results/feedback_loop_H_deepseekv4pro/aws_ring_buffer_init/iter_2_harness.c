#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buf != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    /* Bound size to avoid excessive memory allocation and pointer overflow */
    __CPROVER_assume(size > 0 && size <= 4096);

    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* Check that result is a valid return code */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Success: allocation must be non-NULL */
        assert(ring_buf->allocation != NULL);
        /* allocator must be set */
        assert(ring_buf->allocator == allocator);
        /* allocation_end must be allocation + size */
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        /* The ring buffer should be valid */
        assert(aws_ring_buffer_is_valid(ring_buf));
    } else {
        /* Failure: allocation must be NULL */
        assert(ring_buf->allocation == NULL);
        /* If the function zeroes the struct on failure, these hold; otherwise they may fail,
           which is a valid verification result. */
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
    }

    /* Clean up */
    if (result == AWS_OP_SUCCESS) {
        aws_ring_buffer_clean_up(ring_buf);
    }
    free(ring_buf);
}
