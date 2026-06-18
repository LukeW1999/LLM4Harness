#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_uint64_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    aws_ring_buffer_init(&ring, allocator, ring_size);

    /* Prepare a byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* Verify ring buffer state after release */
    assert(ring.allocation == NULL);
    assert(ring.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring.head) == 0);
    assert(aws_atomic_load_int(&ring.tail) == 0);
    assert(ring.size == 0);
    assert(ring.allocator == old_ring.allocator);

    /* Verify byte buffer state after release */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The ring buffer should still be valid (empty) */
    assert(aws_ring_buffer_is_valid(&ring));
}
