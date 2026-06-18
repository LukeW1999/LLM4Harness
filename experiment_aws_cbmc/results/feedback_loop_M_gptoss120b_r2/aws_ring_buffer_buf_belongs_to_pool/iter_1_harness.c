#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Allocate and initialize a ring buffer with bounded size */
    struct aws_ring_buffer ring;
    ring.allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    int init_res = aws_ring_buffer_init(&ring, aws_default_allocator(), ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Allocate and initialize a byte buffer with bounded capacity */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();

    size_t buf_cap = nondet_size_t();
    __CPROVER_assume(buf_cap <= MAX_BUFFER_SIZE);
    uint8_t *buf_mem = malloc(buf_cap);
    __CPROVER_assume(buf_mem != NULL || buf_cap == 0);
    buf.buffer = buf_mem;
    buf.capacity = buf_cap;
    buf.len = 0;

    /* 3. Possibly point the byte buffer into the ring buffer's allocation */
    bool inside = nondet_bool();
    if (inside) {
        __CPROVER_assume(ring_size >= buf_cap && buf_cap > 0);
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset + buf_cap <= ring_size);
        buf.buffer = ring.allocation + offset;
    }

    /* 4. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 5. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 6. Post‑condition: result reflects whether buf lies within the ring allocation */
    bool in_range = (buf.buffer >= ring.allocation) &&
                    (buf.buffer + buf.capacity <= ring.allocation_end);
    assert(result == in_range);

    /* 7. Unchanged fields on both success and failure */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);

    /* 8. Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
}
