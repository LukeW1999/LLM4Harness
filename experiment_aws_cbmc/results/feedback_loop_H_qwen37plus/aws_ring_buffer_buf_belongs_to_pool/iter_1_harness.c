#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(aws_atomic_load_size(&ring_buf.head) == aws_atomic_load_size(&old_ring_buf.head));
    assert(aws_atomic_load_size(&ring_buf.tail) == aws_atomic_load_size(&old_ring_buf.tail));
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));
}
