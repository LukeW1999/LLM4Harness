#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up a ring buffer */
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + size;
    ring.allocator = alloc;
    ring.head = (struct aws_atomic_var){ .value = 0 };
    ring.tail = (struct aws_atomic_var){ .value = 0 };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Set up a byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Choose whether the buffer points inside or outside the ring allocation */
    bool inside = nondet_bool();
    if (inside) {
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset < size);
        buf.buffer = ring.allocation + offset;
    } else {
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset > 0 && offset <= MAX_BUFFER_SIZE);
        buf.buffer = ring.allocation_end + offset;
    }

    /* 4. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 5. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 6. Post‑condition assertions */
    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    /* 7. Frame‑condition assertions (unchanged state) */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head.value == old_ring.head.value);
    assert(ring.tail.value == old_ring.tail.value);

    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* 8. Invariant assertions */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
