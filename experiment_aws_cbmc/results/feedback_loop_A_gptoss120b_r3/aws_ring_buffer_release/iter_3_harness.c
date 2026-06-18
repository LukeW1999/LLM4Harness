#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    /* Data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* Allocate ring buffer memory */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = aws_default_allocator();
    ring.head = 0;
    ring.tail = 0;
    ring.size = ring_size;

    /* Initialise a byte buffer that points into the ring buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    buf.buffer = ring.allocation + offset;

    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.capacity = cap;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= cap);
    buf.len = len;

    buf.allocator = ring.allocator;

    /* Preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring, &buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* Post‑conditions */

    /* Tail should advance by the released capacity (wrapping if necessary) */
    size_t expected_tail = old_ring.tail + old_buf.capacity;
    if (expected_tail >= old_ring.size) {
        expected_tail -= old_ring.size;
    }
    assert(ring.tail == expected_tail);

    /* The released byte buffer must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Ring fields other than tail remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.size == old_ring.size);

    /* Invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
