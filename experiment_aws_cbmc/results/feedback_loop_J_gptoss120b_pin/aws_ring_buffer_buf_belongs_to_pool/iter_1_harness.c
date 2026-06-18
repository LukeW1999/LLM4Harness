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
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Allocate and bound the ring buffer's internal allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + ring_size;

    /* Initialize other ring buffer fields */
    ring_buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_atomic_load_int(&ring_buf.head) == 0);
    __CPROVER_assume(aws_atomic_load_int(&ring_buf.tail) == 0);
    /* Ensure the ring buffer satisfies its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Allocate and bound the byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* The buffer must lie within the ring buffer's allocation region */
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer + buf.capacity <= ring_buf.allocation_end);
    } else {
        /* The buffer is not wholly contained in the allocation region */
        assert(!(buf.buffer >= ring_buf.allocation &&
                 buf.buffer + buf.capacity <= ring_buf.allocation_end));
    }
