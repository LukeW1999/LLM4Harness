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
    struct aws_byte_buf buf;

    /* Allocate ring buffer memory */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring.allocation = malloc(ring_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_size;
    ring.allocator = aws_default_allocator();

    /* Initialise atomic head and tail to some valid pointers inside the allocation */
    uint8_t *head_ptr = ring.allocation;
    uint8_t *tail_ptr = ring.allocation;
    /* The actual representation of aws_atomic_var is opaque; we set it via the provided macro */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring, tail_ptr);

    /* Ensure the byte buffer has an allocated buffer member */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Choose a buffer that belongs to the ring buffer */
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

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Assume the buffer indeed belongs to the pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring, &buf));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 4. Assert postconditions */

    /* The tail pointer must now point to the end of the released buffer */
    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_buf.buffer + old_buf.capacity);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Unchanged fields of the ring buffer (except tail) */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring));

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
