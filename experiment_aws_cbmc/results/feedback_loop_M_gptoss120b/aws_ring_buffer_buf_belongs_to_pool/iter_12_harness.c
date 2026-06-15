#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

extern uint64_t nondet_uint64_t(void);

static void initialize_ring_buffer(struct aws_ring_buffer *ring) {
    size_t size = (size_t)nondet_uint64_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    __CPROVER_assume((size & (size - 1)) == 0); /* power‑of‑two */

    struct aws_allocator *allocator = aws_default_allocator();
    ring->allocator = allocator;
    ring->allocation = (uint8_t *)aws_mem_acquire(allocator, size);
    __CPROVER_assume(ring->allocation != NULL);
    ring->allocation_end = ring->allocation + size;
    ring->size = size;

    aws_atomic_init_int(&ring->head, 0);
    aws_atomic_init_int(&ring->tail, 0);

    ring->mask = size - 1;
    ring->read_cursor = 0;
    ring->write_cursor = 0;

    __CPROVER_assume(aws_ring_buffer_is_valid(ring));
}

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    initialize_ring_buffer(&ring);

    struct aws_allocator *old_allocator = ring.allocator;
    uint8_t *old_allocation = ring.allocation;
    uint8_t *old_allocation_end = ring.allocation_end;
    size_t old_size = ring.size;
    size_t old_mask = ring.mask;
    size_t old_read_cursor = ring.read_cursor;
    size_t old_write_cursor = ring.write_cursor;
    int old_head = aws_atomic_load_int(&ring.head);
    int old_tail = aws_atomic_load_int(&ring.tail);

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    uint8_t *old_buf_ptr = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer + buf.capacity <= ring.allocation_end);
    } else {
        assert(!((buf.buffer >= ring.allocation) &&
                 (buf.buffer + buf.capacity <= ring.allocation_end)));
    }

    assert(ring.allocator == old_allocator);
    assert(ring.allocation == old_allocation);
    assert(ring.allocation_end == old_allocation_end);
    assert(ring.size == old_size);
    assert(ring.mask == old_mask);
    assert(ring.read_cursor == old_read_cursor);
    assert(ring.write_cursor == old_write_cursor);
    assert(aws_atomic_load_int(&ring.head) == old_head);
    assert(aws_atomic_load_int(&ring.tail) == old_tail);

    assert(buf.buffer == old_buf_ptr);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.allocator == old_buf_allocator);

    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
