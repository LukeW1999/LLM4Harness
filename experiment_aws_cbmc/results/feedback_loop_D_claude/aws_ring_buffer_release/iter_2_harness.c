#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness(void) {
    /* Allocate and set up ring_buffer */
    struct aws_ring_buffer ring_buffer;

    /* Set up a valid ring buffer with a real allocation */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + buf_size;

    /* Initialize head and tail atomics to point within allocation */
    size_t head_offset;
    __CPROVER_assume(head_offset <= buf_size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(allocation + head_offset));

    size_t tail_offset;
    __CPROVER_assume(tail_offset <= buf_size);
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(allocation + tail_offset));

    /* Set up buf - buffer must point within ring_buffer's allocation,
       and buffer + capacity must also be within allocation */
    struct aws_byte_buf buf;
    size_t buf_capacity;
    size_t buf_offset;
    __CPROVER_assume(buf_offset <= buf_size);
    __CPROVER_assume(buf_capacity <= buf_size - buf_offset);
    buf.buffer = allocation + buf_offset;
    buf.capacity = buf_capacity;
    size_t nondet_len;
    __CPROVER_assume(nondet_len <= buf_capacity);
    buf.len = nondet_len;
    buf.allocator = NULL;

    /* The new tail will be buf->buffer + buf->capacity */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Save old ring_buffer state */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buffer.head);

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* === ASSERTIONS: Frame conditions on ring_buffer === */
    /* allocator: UNCHANGED */
    assert(ring_buffer.allocator == old_allocator);
    /* allocation: UNCHANGED */
    assert(ring_buffer.allocation == old_allocation);
    /* allocation_end: UNCHANGED */
    assert(ring_buffer.allocation_end == old_allocation_end);
    /* head: UNCHANGED */
    assert(aws_atomic_load_ptr(&ring_buffer.head) == old_head);
    /* tail: CHANGED to buf->buffer + buf->capacity (before zeroing) */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)expected_new_tail);

    /* === ASSERTIONS: buf is zeroed === */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* === ASSERTIONS: ring_buffer validity === */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
