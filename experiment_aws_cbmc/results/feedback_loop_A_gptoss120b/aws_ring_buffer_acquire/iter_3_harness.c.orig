#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024U

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_ring_buffer_acquire_harness(void) {
    /* ---------- 1. Allocator and ring buffer ---------- */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_ring_buffer ring_buf;
    assert(aws_ring_buffer_init(&ring_buf, alloc, ring_size) == AWS_OP_SUCCESS);

    /* ---------- 2. Initialise head and tail offsets ---------- */
    size_t head_off = nondet_size_t();
    size_t tail_off = nondet_size_t();
    __CPROVER_assume(head_off < ring_size);
    __CPROVER_assume(tail_off < ring_size);

    aws_atomic_store_int(&ring_buf.head, (int)head_off);
    aws_atomic_store_int(&ring_buf.tail, (int)tail_off);

    /* ---------- 3. Compute used / available space ---------- */
    size_t used = (head_off >= tail_off) ? (head_off - tail_off)
                                         : (ring_size - (tail_off - head_off));
    size_t available = ring_size - used;

    /* ---------- 4. Destination byte buffer ---------- */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* ---------- 5. Request size with both outcomes reachable ---------- */
    size_t requested_size = nondet_size_t();
    bool want_success = nondet_bool();

    if (want_success) {
        __CPROVER_assume(requested_size > 0);
        __CPROVER_assume(requested_size <= available);
    } else {
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
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(aws_atomic_load_int(&ring_buf.head) == aws_atomic_load_int(&old_ring.head));
        assert(aws_atomic_load_int(&ring_buf.tail) == aws_atomic_load_int(&old_ring.tail));

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
