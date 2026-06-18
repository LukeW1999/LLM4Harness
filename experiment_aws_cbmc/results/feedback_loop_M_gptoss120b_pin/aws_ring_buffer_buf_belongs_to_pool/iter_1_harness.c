#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare and bound ring buffer */
    struct aws_ring_buffer ring;
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    /* Initialize ring buffer with the default allocator */
    int init_res = aws_ring_buffer_init(&ring, aws_default_allocator(), ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Postconditions for success */
    if (result) {
        /* The buffer must point inside the ring buffer allocation region */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        /* If false, the buffer does NOT point inside the allocation region */
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    /* 5. Unchanged fields of ring buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    /* 6. Unchanged fields of byte buffer */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 7. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring);
}
