#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int main() {
    /* 1. Declare and initialize ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);

    int init_res = aws_ring_buffer_init(&ring_buf, alloc, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save immutable fields */
    struct aws_ring_buffer old = ring_buf;

    /* Save old head and tail pointers */
    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);

    /* 2. Prepare destination byte buffer */
    struct aws_byte_buf dest;

    /* 3. Nondeterministic requested size (must be non‑zero) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Load new head and tail after the call */
    uint8_t *new_head = NULL;
    uint8_t *new_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* head must have advanced */
        assert(new_head != old_head);
        /* tail must be unchanged */
        assert(new_tail == old_tail);
        /* head stays within allocation bounds */
        assert(new_head >= ring_buf.allocation);
        assert(new_head <= ring_buf.allocation_end);

        /* dest describes the acquired buffer */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
    } else {
        /* on failure, head and tail must be unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }

    /* 6. Fields that never change */
    assert(ring_buf.allocator == old.allocator);
    assert(ring_buf.allocation == old.allocation);
    assert(ring_buf.allocation_end == old.allocation_end);

    /* 7. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    return 0;
}
