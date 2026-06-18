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
    struct aws_byte_buf buf;

    /* Allocate a bounded region for the ring buffer's internal allocation */
    size_t ring_alloc_size = nondet_size_t();
    __CPROVER_assume(ring_alloc_size > 0);
    __CPROVER_assume(ring_alloc_size <= MAX_BUFFER_SIZE);
    uint8_t *ring_allocation = malloc(ring_alloc_size);
    __CPROVER_assume(ring_allocation != NULL);

    ring.allocation = ring_allocation;
    ring.allocation_end = ring_allocation + ring_alloc_size;
    ring.allocator = aws_default_allocator();

    /* The atomic fields are left nondeterministic; validity will constrain them */
    /* No explicit bounding helper for aws_ring_buffer, rely on its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Ensure the byte buffer has an allocated backing store and is bounded */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* The buffer's memory must lie within the ring's allocation region */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        /* If false, the buffer's memory is not within the ring's allocation region */
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }
