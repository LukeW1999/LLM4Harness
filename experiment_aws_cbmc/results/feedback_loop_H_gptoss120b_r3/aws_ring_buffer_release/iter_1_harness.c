#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size < MAX_BUFFER_SIZE);

    /* Initialize the ring buffer (must succeed for the harness) */
    int init_res = aws_ring_buffer_init(&ring, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    /* Ensure buf points inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset < ring_size);
    buf.buffer = ring.allocation + offset;

    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.capacity = cap;

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    buf.allocator = NULL; /* allocator not used for release */

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_ring_buffer_check_atomic_ptr(&ring, buf.buffer + buf.capacity));

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

    /* tail pointer updated to point just past the released region */
    assert(AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_buf.buffer + old_buf.capacity);

    /* unchanged fields of the ring buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring));

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
