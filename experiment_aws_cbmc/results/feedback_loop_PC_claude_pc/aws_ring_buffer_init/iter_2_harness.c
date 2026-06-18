#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size > 0 && size <= 16);

    /* Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Assert postconditions based on result */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);
        assert(ring_buf.allocation_end > ring_buf.allocation);
        assert(aws_ring_buffer_is_valid(&ring_buf));

        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        assert(result == AWS_OP_ERR);
        assert(ring_buf.allocation == NULL);
    }
}
