#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_RING_BUFFER_ALLOCATION 10
#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    /* Set up a valid ring buffer with a guaranteed backing allocation */
    ensure_ring_buffer_has_allocated_member(&ring_buf, MAX_RING_BUFFER_ALLOCATION);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up a valid byte_buf (buffer will be overwritten on success) */
    ensure_byte_buf_has_allocated_buffer_member(&dest, MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* requested_size: positive, bounded */
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* Save state before the call */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    struct aws_byte_buf old_dest = dest;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Read back the possibly modified atomic pointers */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(new_head != old_head);          /* head moved forward */
        assert(new_tail == old_tail);         /* tail unchanged */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(old_head == new_head);
        assert(old_tail == new_tail);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Free the resources that were allocated by the helpers */
    free(ring_buf.allocation);
    free(old_dest.buffer);
}
