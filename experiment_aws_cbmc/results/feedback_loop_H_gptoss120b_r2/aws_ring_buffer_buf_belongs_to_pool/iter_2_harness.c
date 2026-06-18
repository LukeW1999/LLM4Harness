#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    /* nondeterministic size for the ring buffer allocation, bounded */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size < MAX_BUFFER_SIZE);

    uint8_t ring_storage[MAX_BUFFER_SIZE];
    ring.allocation = ring_storage;
    ring.allocation_end = ring_storage + ring_size;

    /* initialize atomic head/tail */
    aws_atomic_store_int(&ring.head, 0);
    aws_atomic_store_int(&ring.tail, 0);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert unchanged fields (function is pure) */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_int(&ring.head) == aws_atomic_load_int(&old_ring.head));
    assert(aws_atomic_load_int(&ring.tail) == aws_atomic_load_int(&old_ring.tail));

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 5. Postcondition on return value */
    if (result) {
        /* The buffer must point inside the ring buffer's allocation */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        /* If false, the buffer does NOT lie inside the allocation range */
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    /* 6. Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
