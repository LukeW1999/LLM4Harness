#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Allocate and initialize a valid ring buffer */
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    int init_res = aws_ring_buffer_init(&ring, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Create a byte buffer that belongs to the ring buffer pool */
    struct aws_byte_buf buf;
    /* Ensure the struct fields are allocated (buffer pointer will be overwritten) */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size);
    __CPROVER_assume(cap > 0);

    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + cap <= ring_size);

    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = alloc;

    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring, &buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;
    void *old_head = aws_atomic_load_ptr(&old_ring.head);
    void *old_tail = aws_atomic_load_ptr(&old_ring.tail);

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Post‑conditions: changed fields */
    assert(aws_atomic_load_ptr(&ring.tail) == (void *)(old_buf.buffer + old_buf.capacity));

    /* buf is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 6. Unchanged fields of the ring buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring.head) == old_head);
    /* tail changed intentionally, other fields remain the same */

    /* 7. Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring));
}
