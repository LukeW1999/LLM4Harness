#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* Allocate and bound the ring buffer's allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = aws_default_allocator();

    /* Initialize atomic vars to a nondet but bounded state */
    ring.head = (struct aws_atomic_var){0};
    ring.tail = (struct aws_atomic_var){0};

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Allocate and bound the byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Make the byte buffer point inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    size_t cap = nondet_size_t();
    __CPROVER_assume(offset + cap <= ring_size);
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= cap);
    buf.allocator = ring.allocator;

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 4. Assert postconditions */

    /* buf is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* ring buffer fields that must not change */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* head atomic variable must remain unchanged */
    assert(((void * *)(&ring.head))[0] == ((void * *)(&old_ring.head))[0]);

    /* tail atomic variable must now point to the end of the released buffer */
    assert(((void * *)(&ring.tail))[0] == (void *)(old_buf.buffer + old_buf.capacity));

    /* 5. Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring));
}
