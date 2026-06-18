#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_release_harness(void) {
    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    uint8_t ring_storage[MAX_BUFFER_SIZE];

    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    ring_buffer.allocation = ring_storage;
    ring_buffer.allocation_end = ring_storage + ring_size;
    ring_buffer.allocator = allocator;
    ring_buffer.size = ring_size;
    ring_buffer.is_power_of_two = (ring_size & (ring_size - 1)) == 0;
    ring_buffer.mask = ring_buffer.is_power_of_two ? ring_size - 1 : 0;

    aws_atomic_init_ptr(&ring_buffer.head, (void *)ring_storage);
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)ring_storage);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    size_t buf_capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= ring_size);

    size_t offset = (size_t)nondet_uint64_t();
    __CPROVER_assume(offset + buf_capacity <= ring_size);

    buf.buffer = ring_storage + offset;
    buf.capacity = buf_capacity;
    buf.len = (size_t)nondet_uint64_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring_buffer, &buf);

    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf.buffer + old_buf.capacity), "tail advanced");
    __CPROVER_assert(buf.buffer == NULL, "buf.buffer cleared");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity cleared");
    __CPROVER_assert(buf.len == 0, "buf.len cleared");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator cleared");
    __CPROVER_assert(ring_buffer.allocator == old_ring.allocator, "allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_ring.allocation, "allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_ring.allocation_end, "allocation_end unchanged");
    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring.head), "head unchanged");
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer remains valid");
}
