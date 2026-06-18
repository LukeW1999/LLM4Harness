#include <stdbool.h>
#include <assert.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    int init_rc = aws_ring_buffer_init(&ring, allocator, alloc_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);

    /* Set head and tail to zero */
    ring.head = 0;
    ring.tail = 0;

    /* Prepare a byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save old states */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    /* Verify that the ring buffer state is unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);
    assert(ring.size == old_ring.size);

    /* Verify that the byte buffer state is unchanged */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len <= buf.capacity);

    /* Additional invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
