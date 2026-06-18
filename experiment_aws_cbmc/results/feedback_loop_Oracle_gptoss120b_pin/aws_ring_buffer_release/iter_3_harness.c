#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* Allocate and nondeterministically initialize a ring buffer */
    struct aws_ring_buffer ring_buf;
    ensure_ring_buffer_has_allocated(&ring_buf, aws_default_allocator(), nondet_size_t());

    /* Basic validity */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Load head and tail pointers */
    uint8_t *head_ptr = AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf);
    uint8_t *tail_ptr = AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf);

    /* Ensure pointers are inside the allocation */
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr <= ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr <= ring_buf.allocation_end);

    /* Simplify: assume no wrap (head >= tail) so the region to release is contiguous */
    __CPROVER_assume(head_ptr >= tail_ptr);
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));

    /* Choose a size for the buffer to release that fits within the contiguous region */
    size_t release_size = nondet_size_t();
    __CPROVER_assume(release_size > 0);
    __CPROVER_assume(release_size <= (size_t)(head_ptr - tail_ptr));

    /* Ensure the release does not cross the allocation end (no wrap after release) */
    __CPROVER_assume((size_t)(tail_ptr - ring_buf.allocation) + release_size
                     <= (size_t)(ring_buf.allocation_end - ring_buf.allocation));

    /* Construct a byte buffer that represents the slice to be released */
    struct aws_byte_buf buf;
    buf.buffer = tail_ptr;
    buf.capacity = release_size;
    buf.len = release_size;
    buf.allocator = ring_buf.allocator;
#if defined(AWS_BYTE_BUF_MAX_CAPACITY)
    buf.max_capacity = release_size;
#endif
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for post‑condition checks */
    uint8_t *old_tail_ptr = tail_ptr;
    uint8_t *old_head_ptr = head_ptr;
    uint8_t *old_allocation      = ring_buf.allocation;
    uint8_t *old_allocation_end  = ring_buf.allocation_end;
    size_t   old_buf_capacity    = buf.capacity;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buf, &buf);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks                                                */
    /* -------------------------------------------------------------------- */

    /* 1. Tail pointer must be updated to point just past the released buffer */
    uint8_t *expected_tail = old_tail_ptr + old_buf_capacity;
    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf) == expected_tail);

    /* 2. The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
#if defined(AWS_BYTE_BUF_MAX_CAPACITY)
    assert(buf.max_capacity == 0);
#endif

    /* 3. Ring buffer fields other than the tail must remain unchanged */
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf) == old_head_ptr);

    /* 4. The ring buffer must still satisfy its validity predicate */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
