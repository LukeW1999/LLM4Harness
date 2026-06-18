#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* Allocate and initialize ring buffer fields */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = aws_default_allocator();

    /* Initialize atomic head and tail to the start of the allocation */
    ring.head.value = (void *)ring.allocation;
    ring.tail.value = (void *)ring.allocation;

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Prepare a byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 4. Assert postconditions */

    /* Tail pointer should be updated to point just past the released region */
    assert((uint8_t *)ring.tail.value == old_buf.buffer + old_buf.capacity);

    /* All other ring buffer fields must remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert((uint8_t *)ring.head.value == (uint8_t *)old_ring.head.value);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 5. Validity invariant must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
}
