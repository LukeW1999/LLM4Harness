// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_clean_up returns AWS_OP_SUCCESS (or the successful value):
//   - ring_buf->allocator: UNCHANGED
//   - ring_buf->allocation: SET TO NULL
//   - ring_buf->head: SET TO 0
//   - ring_buf->tail: SET TO 0
//   - ring_buf->allocation_end: SET TO NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_clean_up returns AWS_OP_ERR (or fails):
//   - ring_buf->allocator: UNCHANGED
//   - ring_buf->allocation: UNCHANGED
//   - ring_buf->head: UNCHANGED
//   - ring_buf->tail: UNCHANGED
//   - ring_buf->allocation_end: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   ring_buf (struct aws_ring_buffer):
//     - allocator: UNCHANGED always
//     - allocation: CHANGED on success, UNCHANGED on failure
//     - head: CHANGED on success, UNCHANGED on failure
//     - tail: CHANGED on success, UNCHANGED on failure
//     - allocation_end: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buf): YES (must hold after call)

#include <aws/common/ring_buffer.h>
#include <aws/common/allocators.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf = ring_buf;

    ring_buf.allocator = (struct aws_allocator *)nondet_ptr();
    ring_buf.allocation = (uint8_t *)nondet_ptr();
    ring_buf.head = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buf.allocation_end = (uint8_t *)nondet_ptr();

    aws_ring_buffer_clean_up(&ring_buf);

    // Assertions for success path
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert((ring_buf.allocation == NULL && ring_buf.head.var == 0 && ring_buf.tail.var == 0 && ring_buf.allocation_end == NULL) ||
           (ring_buf.allocation == old_ring_buf.allocation && ring_buf.head.var == old_ring_buf.head.var && ring_buf.tail.var == old_ring_buf.tail.var && ring_buf.allocation_end == old_ring_buf.allocation_end));

    // Validity invariant
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
