#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024U

int nondet_int(void);
size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_ring_buffer_acquire_harness(void) {
    /* ---------- 1. Set up allocator and ring buffer ---------- */
    struct aws_allocator *alloc = aws_default_allocator();
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_ring_buffer ring_buf;
    assert(aws_ring_buffer_init(&ring_buf, alloc, ring_size) == AWS_OP_SUCCESS);

    /* ---------- 2. Initialise head and tail positions ---------- */
    size_t head_off = nondet_size_t();
    size_t tail_off = nondet_size_t();
    __CPROVER_assume(head_off <= ring_size);
    __CPROVER_assume(tail_off <= ring_size);

    ring_buf.head = (struct aws_atomic_var){ .value = ring_buf.allocation + head_off };
    ring_buf.tail = (struct aws_atomic_var){ .value = ring_buf.allocation + tail_off };

    /* ---------- 3. Compute available space ---------- */
    size_t used = (head_off >= tail_off) ? (head_off - tail_off)
                                         : (ring_size - (tail_off - head_off));
    size_t available = ring_size - used;

    /* ---------- 4. Prepare destination byte buffer ---------- */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* ---------- 5. Non‑deterministic request size with both outcomes reachable ---------- */
    size_t requested_size = nondet_size_t();
    bool want_success = nondet_bool();

    if (want_success) {
        __CPROVER_assume(requested_size > 0);
        __CPROVER_assume(requested_size <= available);
    } else {
        /* Force failure: either request zero or more than available */
        __CPROVER_assume(requested_size == 0 || requested_size > available);
    }

    /* ---------- 6. Save old state ---------- */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* ---------- 7. Call function under test ---------- */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* ---------- 8. Post‑condition checks ---------- */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a region inside the ring buffer */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.len == requested_size);

        /* allocator and allocation must stay unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);

        /* head and tail must stay within bounds */
        assert(ring_buf.head.value >= ring_buf.allocation);
        assert(ring_buf.head.value <= ring_buf.allocation_end);
        assert(ring_buf.tail.value >= ring_buf.allocation);
        assert(ring_buf.tail.value <= ring_buf.allocation_end);
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.head.value == old_ring.head.value);
        assert(ring_buf.tail.value == old_ring.tail.value);

        /* Destination buffer must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* ---------- 9. Global invariants ---------- */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
