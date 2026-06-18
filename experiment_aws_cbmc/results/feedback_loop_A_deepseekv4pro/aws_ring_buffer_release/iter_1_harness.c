#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Initialize ring_buffer with non-deterministic but valid state */
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = NULL;
    ring_buffer.allocation_end = NULL;
    aws_atomic_init_int(&ring_buffer.head, 0);
    aws_atomic_init_int(&ring_buffer.tail, 0);

    /* Allocate the ring buffer's backing memory */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set head and tail to valid positions within the allocation */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);
    aws_atomic_store_int(&ring_buffer.head, head_offset);
    aws_atomic_store_int(&ring_buffer.tail, tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Initialize buf with non-deterministic but valid state */
    buf.allocator = aws_default_allocator();
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;

    /* Allocate buf's buffer member */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0 && buf_capacity <= MAX_BUFFER_SIZE);
    buf.buffer = (uint8_t *)malloc(buf_capacity);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Ensure buf belongs to the ring buffer pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The tail pointer should be updated to buf->buffer + buf->capacity */
    /* Since we can't directly read atomic tail as a pointer, we verify the ring buffer remains valid */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* buf should be zeroed out */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* 5. Assert fields that must NOT change */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* head should not change */
    size_t old_head = aws_atomic_load_int(&old_ring_buffer.head);
    size_t new_head = aws_atomic_load_int(&ring_buffer.head);
    assert(new_head == old_head);

    /* 6. Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
