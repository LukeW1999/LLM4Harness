#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer = {0};
    struct aws_allocator *allocator = aws_default_allocator();

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = bounded_malloc(allocation_size);
    __CPROVER_assume(allocation != NULL);

    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);

    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset < allocation_size);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= allocation_size - tail_offset);

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + allocation_size;

    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));

    struct aws_byte_buf buf = {0};
    buf.allocator = NULL;
    buf.buffer = ring_buffer.allocation + tail_offset;
    buf.len = nondet_size_t();
    buf.capacity = buf_capacity;
    __CPROVER_assume(buf.len <= buf.capacity);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)buf.buffer);

    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_head = aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_tail = aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;

    aws_ring_buffer_release(&ring_buffer, &buf);

    uint8_t *expected_tail = old_buf_buffer + old_buf_capacity;
    if (expected_tail == old_allocation_end) {
        expected_tail = old_allocation;
    }

    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)expected_tail, "tail is updated correctly");
    __CPROVER_assert(buf.len == 0, "released buffer len is zero");
    __CPROVER_assert(buf.buffer == NULL, "released buffer pointer is null");
    __CPROVER_assert(buf.capacity == 0, "released buffer capacity is zero");
    __CPROVER_assert(buf.allocator == NULL, "released buffer allocator is null");

    __CPROVER_assert(ring_buffer.allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_allocation, "allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_allocation_end, "allocation end unchanged");
    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.head) == (void *)old_head, "head unchanged");
    __CPROVER_assert(old_tail == old_buf_buffer, "old tail matched released buffer");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "released byte buffer is valid");
}
