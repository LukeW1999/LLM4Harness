// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_buf_belongs_to_pool returns true:
//   - ring_buffer: UNCHANGED
//   - buf: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_buf_belongs_to_pool returns false:
//   - ring_buffer: UNCHANGED
//   - buf: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buffer (struct aws_ring_buffer):
//   - allocator: UNCHANGED always
//   - allocation: UNCHANGED always
//   - head: UNCHANGED always
//   - tail: UNCHANGED always
//   - allocation_end: UNCHANGED always
// buf (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buffer): YES (must hold after call)
//   - aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE): YES (assuming MAX_BUFFER_SIZE is defined)
```

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    // Initialize ring_buffer and buf with arbitrary values
    ring_buffer.allocator = (struct aws_allocator *)nondet_ptr();
    ring_buffer.allocation = (uint8_t *)nondet_ptr();
    ring_buffer.head = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buffer.tail = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buffer.allocation_end = (uint8_t *)nondet_ptr();
    buf.buffer = (uint8_t *)nondet_ptr();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_ptr();

    // Ensure buf is bounded
    assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    // Call the function under test
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    // Assertions for frame conditions and validity invariants
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head.var == old_ring_buffer.head.var);
    assert(ring_buffer.tail.var == old_ring_buffer.tail.var);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
