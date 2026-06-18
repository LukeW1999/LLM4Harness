#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    ring.allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    int init_res = aws_ring_buffer_init(&ring, ring.allocator, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* bound len */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 5. Postconditions */

    /* Success/failure is not applicable (function always returns bool),
       but we assert the logical relationship */
    bool expected = false;
    if (buf.buffer != NULL && ring.allocation != NULL && ring.allocation_end != NULL) {
        if (buf.buffer >= ring.allocation &&
            (size_t)(buf.buffer - ring.allocation) + buf.len <= (size_t)(ring.allocation_end - ring.allocation)) {
            expected = true;
        }
    }
    assert(result == expected);

    /* Unchanged fields of ring buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    /* Unchanged fields of byte buffer */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    /* aws_byte_buf does not have an explicit validity predicate; boundedness already assumed */

    return 0;
}
