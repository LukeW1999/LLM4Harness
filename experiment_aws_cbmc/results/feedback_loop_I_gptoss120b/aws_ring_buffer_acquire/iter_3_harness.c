#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* structures */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest;

    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondet buffer size: power of two, >0, bounded */
    size_t buf_size = nondet_size_t();
    __CPROVER_assume(buf_size > 0);
    __CPROVER_assume(buf_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume((buf_size & (buf_size - 1)) == 0); /* power of two */

    /* allocate ring buffer memory */
    ring_buf.allocation = malloc(buf_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + buf_size;
    ring_buf.size = buf_size;
    ring_buf.mask = buf_size - 1;

    /* initialise head and tail */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);

    /* save old state */
    old_ring_buf = ring_buf;

    /* initialise destination byte buffer (empty) */
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    old_dest = dest;

    /* nondet request size */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* load head/tail after the call */
    uint8_t *new_head = NULL;
    uint8_t *new_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* load old head/tail */
    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring_buf, old_tail);

    /* result must be a valid return code */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* dest points inside the allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        /* length and capacity equal the request */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        /* head advanced correctly (modulo size) */
        size_t old_offset = (size_t)(old_head - ring_buf.allocation);
        size_t expected_offset = (old_offset + requested_size) & ring_buf.mask;
        uint8_t *expected_head = ring_buf.allocation + expected_offset;
        assert(new_head == expected_head);
        /* tail unchanged */
        assert(new_tail == old_tail);
    } else {
        /* ring buffer unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        /* dest unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* invariant checks */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
