#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

#define MAX_SIZE (1024 * 1024)

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size <= MAX_SIZE); /* size may be zero to force error */

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* result must be a known status */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* post‑conditions for successful init */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(ring_buf.allocator == allocator);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* post‑conditions for failed init */
        assert(ring_buf.allocation == NULL);
        /* is_valid may be false on error, no requirement to hold */
    }

    /* allocator pointer must remain unchanged */
    assert(allocator == aws_default_allocator());
}
