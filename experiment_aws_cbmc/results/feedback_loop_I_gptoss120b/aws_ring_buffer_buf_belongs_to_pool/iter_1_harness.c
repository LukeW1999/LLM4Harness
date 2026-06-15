#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + size;
    ring.allocator = alloc;
    ring.head = (struct aws_atomic_var){0};
    ring.tail = (struct aws_atomic_var){0};

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* The buffer must lie within the ring buffer's allocation range */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        /* The buffer does NOT lie within the allocation range */
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
