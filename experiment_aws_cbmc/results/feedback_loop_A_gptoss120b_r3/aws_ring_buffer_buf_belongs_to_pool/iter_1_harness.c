#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    ring.allocator = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = malloc(alloc_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + alloc_size;

    /* head and tail are atomic vars; leave nondet but assume validity later */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert postconditions */

    /* 4a. Result reflects membership of buf.buffer in ring allocation range */
    bool in_range = false;
    if (ring.allocation != NULL && buf.buffer != NULL) {
        in_range = (buf.buffer >= ring.allocation) && (buf.buffer < ring.allocation_end);
    }
    assert(result == in_range);

    /* 4b. Unchanged fields of ring_buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* atomic vars (head, tail) are not expected to change */
    assert(__builtin_memcmp(&ring.head, &old_ring.head, sizeof(ring.head)) == 0);
    assert(__builtin_memcmp(&ring.tail, &old_ring.tail, sizeof(ring.tail)) == 0);

    /* 4c. Unchanged fields of aws_byte_buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
}
