#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf that "belongs" to this ring buffer. */
    struct aws_byte_buf buf;

    size_t offset;
    __CPROVER_assume(offset < ring_size);
    buf.buffer = ring_buffer.allocation + offset;

    size_t remaining = (size_t)(ring_buffer.allocation_end - buf.buffer);
    __CPROVER_assume(buf.capacity <= remaining);
    __CPROVER_assume(buf.capacity > 0);

    /* Ensure new tail stays strictly within allocation bounds */
    __CPROVER_assume(buf.buffer + buf.capacity < ring_buffer.allocation_end);

    size_t nondet_len;
    buf.len = nondet_len;
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old state of ring_buffer fields that should NOT change */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* buf must be zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer fields that must not change */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
