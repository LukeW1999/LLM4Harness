// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_acquire returns AWS_OP_SUCCESS (or the successful value):
//   - ring_buf.head: CHANGES to new head position
//   - ring_buf.tail: UNCHANGED
//   - dest.buffer: CHANGES to point to acquired buffer
//   - dest.len: CHANGES to requested_size
//   - dest.capacity: CHANGES to requested_size
//   - dest.allocator: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_acquire returns AWS_OP_ERR (or fails):
//   - ring_buf.head: UNCHANGED
//   - ring_buf.tail: UNCHANGED
//   - dest.buffer: UNCHANGED
//   - dest.len: UNCHANGED
//   - dest.capacity: UNCHANGED
//   - dest.allocator: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   ring_buf (struct aws_ring_buffer):
//     - allocator: UNCHANGED always
//     - allocation: UNCHANGED always
//     - head: CHANGED on success, UNCHANGED on failure
//     - tail: UNCHANGED always
//     - allocation_end: UNCHANGED always
//   dest (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buf): YES (must hold after call)
//   - aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE): YES (if success)

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size = nondet_size_t();
    int result;

    // Initialize ring_buf with arbitrary values
    ring_buf.allocator = (struct aws_allocator *)nondet_ptr();
    ring_buf.allocation = (uint8_t *)nondet_ptr();
    ring_buf.allocation_end = ring_buf.allocation + nondet_size_t();
    AWS_ATOMIC_INIT_BOOL(ring_buf.head, nondet_size_t());
    AWS_ATOMIC_INIT_BOOL(ring_buf.tail, nondet_size_t());

    // Initialize dest with arbitrary values
    dest.buffer = (uint8_t *)nondet_ptr();
    dest.len = nondet_size_t();
    dest.capacity = nondet_size_t();
    dest.allocator = (struct aws_allocator *)nondet_ptr();

    // Save old state
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    // Assume preconditions
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(requested_size <= (size_t)(ring_buf.allocation_end - ring_buf.allocation));

    // Call function under test
    result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    // Assert frame conditions and validity invariants
    if (result == AWS_OP_SUCCESS) {
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.head) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.head) + requested_size);
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.tail) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.tail));
        assert(dest.buffer == old_ring_buf.allocation);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.allocator == old_dest.allocator);
    } else {
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.head) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.head));
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.tail) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.tail));
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, (size_t)(ring_buf.allocation_end - ring_buf.allocation)));
}
