#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Non-deterministically initialize ring_buffer */
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = nondet_uint8_t_ptr();
    ring_buffer.allocation_end = nondet_uint8_t_ptr();
    /* Bound the allocation size */
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(ring_buffer.allocation_end != NULL);
    __CPROVER_assume(ring_buffer.allocation < ring_buffer.allocation_end);
    __CPROVER_assume((size_t)(ring_buffer.allocation_end - ring_buffer.allocation) <= MAX_BUFFER_SIZE);
    /* head and tail are not used by the function under test, so we leave them uninitialized */

    /* Non-deterministically initialize byte_buf */
    buf.allocator = aws_default_allocator();
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    /* Bound the buffer size */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    /* Assume byte_buf is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks (only the fields relevant to the function) */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Immutability: only fields that could be modified (though function should not modify any) */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* Validity invariants hold after the call */
    /* We reuse aws_byte_buf_is_valid on buf because it does not depend on ring buffer */
    assert(aws_byte_buf_is_valid(&buf));

    /* Basic range condition for the return value:
     * The buffer pointer should be within the ring buffer allocation region. */
    bool buf_in_range = (buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    assert(result == buf_in_range);
}
