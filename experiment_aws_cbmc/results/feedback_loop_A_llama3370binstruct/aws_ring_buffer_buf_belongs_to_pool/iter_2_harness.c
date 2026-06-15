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
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    ensure_byte_buf_has_allocated_buffer_member(&ring_buffer.allocation);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(ring_buffer.allocation_end != NULL);
    __CPROVER_assume(ring_buffer.allocation_end > ring_buffer.allocation);

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.buffer != NULL);
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer + buf.len <= ring_buffer.allocation_end);
    } else {
        assert(buf.buffer < ring_buffer.allocation || buf.buffer + buf.len > ring_buffer.allocation_end);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == aws_default_allocator());
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 7. Test all code branches */
    if (nondet_bool()) {
        ring_buffer.allocation = NULL;
        assert(!aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    }

    if (nondet_bool()) {
        buf.buffer = NULL;
        assert(!aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    }

    if (nondet_bool()) {
        buf.len = 0;
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    }
}
