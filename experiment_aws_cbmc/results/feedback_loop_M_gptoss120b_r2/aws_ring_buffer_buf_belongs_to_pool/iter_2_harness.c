#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    int init_res = aws_ring_buffer_init(&ring, allocator, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    buf.allocator = allocator;

    size_t buf_cap = nondet_size_t();
    __CPROVER_assume(buf_cap <= MAX_BUFFER_SIZE);
    uint8_t *buf_mem = malloc(buf_cap);
    __CPROVER_assume(buf_mem != NULL || buf_cap == 0);
    buf.buffer = buf_mem;
    buf.capacity = buf_cap;
    buf.len = 0;

    bool inside = nondet_bool();
    if (inside) {
        __CPROVER_assume(ring_size >= buf_cap && buf_cap > 0);
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset + buf_cap <= ring_size);
        buf.buffer = ring.allocation + offset;
    }

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    bool in_range = (buf.buffer >= ring.allocation) &&
                    (buf.buffer + buf.capacity <= ring.allocation_end);
    assert(result == in_range);

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);

    assert(aws_ring_buffer_is_valid(&ring));
}
