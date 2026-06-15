#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    ensure_byte_buf_has_allocated_buffer_member(&ring_buf.allocation);
    __CPROVER_assume(ring_buf.allocator == allocator);

    struct aws_ring_buffer old_ring_buf = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocation.buffer == NULL);
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}

int main() {
    aws_ring_buffer_clean_up_harness();
    return 0;
}
