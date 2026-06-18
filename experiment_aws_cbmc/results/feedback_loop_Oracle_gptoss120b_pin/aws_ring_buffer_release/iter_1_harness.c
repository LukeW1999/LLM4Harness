#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* Allocate and nondeterministically initialize a ring buffer */
    struct aws_ring_buffer ring_buf;
    ensure_ring_buffer_has_allocated(&ring_buf, aws_default_allocator(), nondet_size_t());

    /* Allocate and nondeterministically initialize a byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_is_valid(&buf);

    /* Preconditions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for post‑condition checks */
    struct aws_atomic_var old_tail = ring_buf.tail;
    struct aws_atomic_var old_head = ring_buf.head;
    uint8_t *old_allocation      = ring_buf.allocation;
    uint8_t *old_allocation_end  = ring_buf.allocation_end;
    uint8_t *old_buf_ptr         = buf.buffer;
    size_t   old_buf_capacity    = buf.capacity;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buf, &buf);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks                                                */
    /* -------------------------------------------------------------------- */

    /* 1. Tail pointer must be updated to point just past the released buffer */
    uint8_t *expected_tail = old_buf_ptr + old_buf_capacity;
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
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf) == AWS_ATOMIC_LOAD_HEAD_PTR(&old_head));

    /* 4. The ring buffer must still satisfy its validity predicate */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    return 0;
}
