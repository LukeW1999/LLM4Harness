#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buffer;
    AWS_ZERO_STRUCT(ring_buffer);

    uint8_t allocation[MAX_BUFFER_SIZE];

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + allocation_size;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);
    __CPROVER_assume(tail_offset < allocation_size);

    aws_atomic_init_ptr(&ring_buffer.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(allocation + tail_offset));

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    AWS_ZERO_STRUCT(buf);

    size_t buf_offset = nondet_size_t();
    __CPROVER_assume(buf_offset < allocation_size);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= allocation_size - buf_offset);

    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= buf_capacity);

    buf.allocator = NULL;
    buf.buffer = allocation + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = buf_len;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buffer.head);
    void *old_tail = aws_atomic_load_ptr(&ring_buffer.tail);

    struct aws_allocator *old_buf_allocator = buf.allocator;
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    __CPROVER_assert(result, "buf belongs to pool");
    __CPROVER_assert(buf.buffer >= ring_buffer.allocation, "buf starts at or after allocation");
    __CPROVER_assert(buf.buffer < ring_buffer.allocation_end, "buf starts before allocation_end");
    __CPROVER_assert(buf.buffer + buf.capacity <= ring_buffer.allocation_end, "buf ends before allocation_end");

    __CPROVER_assert(ring_buffer.allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_allocation, "allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_allocation_end, "allocation_end unchanged");
    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.head) == old_head, "head unchanged");
    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_tail, "tail unchanged");

    __CPROVER_assert(buf.allocator == old_buf_allocator, "buf allocator unchanged");
    __CPROVER_assert(buf.buffer == old_buf_buffer, "buf buffer unchanged");
    __CPROVER_assert(buf.len == old_buf_len, "buf len unchanged");
    __CPROVER_assert(buf.capacity == old_buf_capacity, "buf capacity unchanged");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring_buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf remains valid");
}
