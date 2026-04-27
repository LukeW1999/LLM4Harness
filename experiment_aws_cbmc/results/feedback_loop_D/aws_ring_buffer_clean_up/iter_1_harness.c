// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_clean_up returns AWS_OP_SUCCESS (or the successful value):
//   - ring_buf->allocation: CHANGES to NULL
//   - ring_buf->head: CHANGES to 0
//   - ring_buf->tail: CHANGES to 0
//   - ring_buf->allocation_end: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_clean_up returns AWS_OP_ERR (or fails):
//   - ring_buf->allocation: UNCHANGED
//   - ring_buf->head: UNCHANGED
//   - ring_buf->tail: UNCHANGED
//   - ring_buf->allocation_end: UNCHANGED
//   - ring_buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buf (struct aws_ring_buffer):
//   - allocator: UNCHANGED always
//   - allocation: CHANGED on success, UNCHANGED on failure
//   - head: CHANGED on success, UNCHANGED on failure
//   - tail: CHANGED on success, UNCHANGED on failure
//   - allocation_end: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buf): YES (must hold after call)
```

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf = ring_buf;

    // Initialize the ring buffer with non-deterministic values
    ring_buf.allocator = (struct aws_allocator *)nondet_size_t();
    ring_buf.allocation = (uint8_t *)nondet_size_t();
    ring_buf.head = (struct aws_atomic_var){.var = nondet_size_t()};
    ring_buf.tail = (struct aws_atomic_var){.var = nondet_size_t()};
    ring_buf.allocation_end = (uint8_t *)nondet_size_t();

    aws_ring_buffer_clean_up(&ring_buf);

    // Assert frame conditions
    assert(ring_buf.allocator == old_ring_buf.allocator);
    if (old_ring_buf.allocation) {
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.head.var == 0);
        assert(ring_buf.tail.var == 0);
        assert(ring_buf.allocation_end == NULL);
    } else {
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.head.var == old_ring_buf.head.var);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    }

    // Assert validity invariant
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
