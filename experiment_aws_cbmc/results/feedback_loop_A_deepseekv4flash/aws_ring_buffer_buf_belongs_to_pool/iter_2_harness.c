#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare parameters */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Non-deterministically initialize ring buffer */
    ensure_ring_buffer_has_allocated_members(&ring_buffer);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Non-deterministically initialize byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* To guarantee both branches are reachable, we nondeterministically choose
     * whether buf.buffer is inside the ring buffer's allocation.
     * This is done by allowing buf.buffer to be either in range or not.
     * Since we already have nondeterministic initialization, it's already possible.
     * But to be explicit, we add an assumption that the result can be true or false.
     * However, we must not over-constrain: we only need one case each.
     * We'll use __CPROVER_assume to allow both. 
     * We'll also add an assumption that buf.buffer is not NULL to avoid trivial false.
     * But that's not necessary.
     */

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postconditions */

    /* 1. Ring buffer unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);

    /* 2. Byte buffer unchanged */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 3. Result consistency with bounds */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* If result is false, buf.buffer is either NULL or outside the range.
         * We cannot assert the inverse because buf.buffer could be NULL.
         * But we can assert that if buf.buffer is within range then result must be true.
         * However, that's a property that should hold universally.
         * We'll test it here to cover the else branch.
         */
        if (buf.buffer != NULL) {
            assert(!(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end));
        }
    }

    /* 4. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
