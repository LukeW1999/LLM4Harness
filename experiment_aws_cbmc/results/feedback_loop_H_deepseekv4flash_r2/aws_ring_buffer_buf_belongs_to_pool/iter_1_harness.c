#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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
    /* Initialize atomic variables with arbitrary values */
    ring_buffer.head = (struct aws_atomic_var)nondet_uint64_t(); /* simplified, typically need proper init */
    ring_buffer.tail = (struct aws_atomic_var)nondet_uint64_t();
    /* Assume ring_buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Non-deterministically initialize byte_buf */
    buf.allocator = aws_default_allocator();
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    /* Bound the buffer size */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    /* Assume byte_buf is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Immutability: all fields must remain unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    /* For atomic fields, compare the underlying integer values */
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* Validity invariants hold after the call */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    /* Basic range condition for the return value:
     * The buffer pointer should be within the ring buffer allocation region.
     * (Note: this is a conservative check; the exact implementation may have additional subtleties.) */
    bool buf_in_range = (buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    assert(result == buf_in_range);
}
