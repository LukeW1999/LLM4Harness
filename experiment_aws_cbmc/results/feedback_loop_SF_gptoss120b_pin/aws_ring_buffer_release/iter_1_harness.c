#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* Symbolic allocation size for the ring buffer */
    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0 && allocation_size < 1024);

    /* Allocate memory for the ring buffer's backing store */
    uint8_t *allocation = malloc(allocation_size);
    __CPROVER_assume(allocation != NULL);

    /* Initialize the ring buffer structure */
    struct aws_ring_buffer ring_buffer;
    struct aws_ring_buffer *ring_buffer_ptr = &ring_buffer;
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + allocation_size;
    aws_atomic_store_int(&ring_buffer.head, 0);
    aws_atomic_store_int(&ring_buffer.tail, 0);

    /* Symbolic offset and capacity for the byte buffer to be released */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= allocation_size);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= allocation_size - offset);
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);

    /* Initialize the byte buffer structure */
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = &buf;
    buf.buffer = allocation + offset;
    buf.capacity = capacity;
    buf.len = len;
    buf.allocator = aws_default_allocator();

    /* Preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer_ptr));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(ring_buffer_ptr, buf_ptr));

    /* Snapshot of expected tail after release */
    uint8_t *expected_tail = buf.buffer + buf.capacity;

    /* Call the function under verification */
    aws_ring_buffer_release(ring_buffer_ptr, buf_ptr);

    /* Postcondition assertions */
    /* ring buffer structure remains valid */
    assert(aws_ring_buffer_is_valid(ring_buffer_ptr));

    /* head must be unchanged (still 0) */
    assert(aws_atomic_load_int(&ring_buffer.head) == 0);

    /* tail must advance to the end of the released buffer */
    assert(aws_atomic_load_int(&ring_buffer.tail) ==
           (int)(expected_tail - ring_buffer.allocation));

    /* allocator, allocation pointers must be unchanged */
    assert(ring_buffer.allocator == aws_default_allocator());
    assert(ring_buffer.allocation == allocation);
    assert(ring_buffer.allocation_end == allocation + allocation_size);

    /* byte buffer must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == aws_default_allocator());
}
