#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet size for the ring buffer allocation, bounded */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    ring.allocator = alloc;
    ring.allocation = malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;

    /* initialise atomic head and tail to the start of the allocation */
    ring.head.value = (void *)ring.allocation;
    ring.tail.value = (void *)ring.allocation;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Prepare a byte buffer that points inside the ring buffer allocation */
    struct aws_byte_buf buf;
    /* Choose a nondet offset within the allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    uint8_t *buf_start = ring.allocation + offset;

    /* Choose a nondet capacity that stays inside the allocation */
    size_t max_cap = ring.allocation_end - buf_start;
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= max_cap);

    /* Choose a nondet length that does not exceed capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    buf.buffer = buf_start;
    buf.allocator = alloc;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 4. Assert postconditions */

    /* Tail pointer must be updated to point just past the released region */
    assert(ring.tail.value == (void *)(old_buf.buffer + old_buf.capacity));

    /* Head pointer must remain unchanged */
    assert(ring.head.value == old_ring.head.value);

    /* Allocator and allocation bounds must remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 5. Invariants: the ring buffer must still be valid */
    assert(aws_ring_buffer_is_valid(&ring));

    return 0;
}
