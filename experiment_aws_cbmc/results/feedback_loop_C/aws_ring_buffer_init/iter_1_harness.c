// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_init returns AWS_OP_SUCCESS (or the successful value):
//   - ring_buf->allocator: CHANGES to allocator
//   - ring_buf->allocation: CHANGES to non-null pointer
//   - ring_buf->head: CHANGES to point to ring_buf->allocation
//   - ring_buf->tail: CHANGES to point to ring_buf->allocation
//   - ring_buf->allocation_end: CHANGES to ring_buf->allocation + size
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_init returns AWS_OP_ERR (or fails):
//   - ring_buf->allocator: UNCHANGED
//   - ring_buf->allocation: CHANGES to NULL
//   - ring_buf->head: UNCHANGED
//   - ring_buf->tail: UNCHANGED
//   - ring_buf->allocation_end: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   ring_buf (struct aws_ring_buffer):
//     - allocator: CHANGED on success, UNCHANGED on failure
//     - allocation: CHANGED always
//     - head: CHANGED on success, UNCHANGED on failure
//     - tail: CHANGED on success, UNCHANGED on failure
//     - allocation_end: CHANGED on success, UNCHANGED on failure
//   allocator (struct aws_allocator, if any):
//     - All fields: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buf): YES (must hold after call)

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    size_t size = nondet_size_t();
    struct aws_ring_buffer old_ring_buf = ring_buf;

    int result = aws_ring_buffer_init(&ring_buf, &allocator, size);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation != NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
    } else {
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring_buf.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring_buf.tail));
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
