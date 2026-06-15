#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    /* Ensure the structure starts in a known state */
    memset(&ring_buf, 0, sizeof(ring_buf));

    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size <= 1024);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Return value must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Success case post‑conditions */
    assert((result == AWS_OP_SUCCESS) ==>
        (ring_buf.allocation != NULL &&
         ring_buf.allocator == allocator &&
         aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation &&
         aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation &&
         ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size &&
         aws_ring_buffer_is_valid(&ring_buf)));

    /* Failure case post‑conditions (fields remain NULL) */
    assert((result == AWS_OP_ERR) ==>
        (ring_buf.allocation == NULL &&
         ring_buf.allocator == NULL &&
         aws_atomic_load_ptr(&ring_buf.head) == NULL &&
         aws_atomic_load_ptr(&ring_buf.tail) == NULL &&
         ring_buf.allocation_end == NULL));
}
