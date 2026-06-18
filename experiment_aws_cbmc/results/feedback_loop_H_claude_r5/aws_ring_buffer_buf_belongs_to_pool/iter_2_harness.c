#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf non-deterministically */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Check that ring_buffer fields are unchanged */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* Check that buf fields are unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Verify the semantic meaning of the result */
    if (result) {
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }

    /* Validity invariants must hold after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
