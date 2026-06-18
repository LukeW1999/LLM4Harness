#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ensure_ring_buffer_has_allocated_buffer(&ring_buf, ring_size);
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    bool in_range = (buf.buffer >= ring_buf.allocation) && (buf.buffer < ring_buf.allocation_end);
    if (result) {
        __CPROVER_assert(in_range, "Result true implies buffer is within ring allocation");
    } else {
        __CPROVER_assert(!in_range, "Result false implies buffer is outside ring allocation");
    }
}
