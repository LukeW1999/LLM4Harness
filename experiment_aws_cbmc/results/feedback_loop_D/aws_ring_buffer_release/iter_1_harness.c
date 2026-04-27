// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_release returns AWS_OP_SUCCESS (or the successful value):
//   - ring_buffer->tail: CHANGES to buf->buffer + buf->capacity
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_release returns AWS_OP_ERR (or fails):
//   - ring_buffer->head: UNCHANGED
//   - ring_buffer->tail: UNCHANGED
//   - ring_buffer->allocator: UNCHANGED
//   - ring_buffer->allocation: UNCHANGED
//   - ring_buffer->allocation_end: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buffer (struct aws_ring_buffer):
//   - allocator: UNCHANGED always
//   - allocation: UNCHANGED always
//   - head: UNCHANGED always
//   - tail: CHANGED on success, UNCHANGED on failure
//   - allocation_end: UNCHANGED always
// buf (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buffer): YES (must hold after call)
//   - aws_byte_buf_is_valid(&buf): NO (buf is zeroed out)
```

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    // Initialize ring_buffer with arbitrary values
    ring_buffer.allocator = (struct aws_allocator *)nondet_ptr();
    ring_buffer.allocation = (uint8_t *)nondet_ptr();
    ring_buffer.head = (struct aws_atomic_var){.var = nondet_uint64_t()};
    ring_buffer.tail = (struct aws_atomic_var){.var = nondet_uint64_t()};
    ring_buffer.allocation_end = (uint8_t *)nondet_ptr();

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)nondet_ptr();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_ptr();

    // Save old state
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    // Call the function under test
    aws_ring_buffer_release(&ring_buffer, &buf);

    // Assert frame conditions and validity invariants
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head.var == old_ring_buffer.head.var);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Tail should change on success
    assert(ring_buffer.tail.var == (uintptr_t)(old_buf.buffer + old_buf.capacity));

    // Check validity invariants
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
