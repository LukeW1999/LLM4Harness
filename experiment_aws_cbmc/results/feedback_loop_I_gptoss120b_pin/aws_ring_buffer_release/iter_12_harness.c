#define AWS_STATIC_IMPL
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    uint8_t storage[256];
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buffer = {0};
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = storage;
    ring_buffer.allocation_end = storage + sizeof(storage);
    aws_atomic_init_ptr(&ring_buffer.head, storage + 16);
    aws_atomic_init_ptr(&ring_buffer.tail, storage);
    ring_buffer.size = 16;
    ring_buffer.capacity = sizeof(storage);

    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    buf.capacity = 16;
    buf.len = buf.capacity;
    buf.allocator = allocator;

    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;

    aws_ring_buffer_release(&ring_buffer, &buf);

    __CPROVER_assert(buf.buffer == NULL, "buf.buffer cleared");
    __CPROVER_assert(buf.len == 0, "buf.len cleared");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity cleared");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator cleared");

    __CPROVER_assert(ring_buffer.allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_allocation, "allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_allocation_end, "allocation_end unchanged");

    __CPROVER_assert(
        aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf_buffer + old_buf_capacity),
        "tail advanced correctly");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer remains valid");
}
