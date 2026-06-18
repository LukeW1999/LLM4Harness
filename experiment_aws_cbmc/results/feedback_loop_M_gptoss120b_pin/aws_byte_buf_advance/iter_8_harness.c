#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(buffer.capacity > 0);
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(buffer.allocator != NULL);               /* allocator must be non‑NULL */

    /* 2. Declare the destination buffer (uninitialized) */
    struct aws_byte_buf output;

    /* 3. Non‑deterministic length, bounded and feasible for advance */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.capacity - buffer.len);   /* must succeed */

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Post‑conditions */
    if (result) {
        /* source buffer advanced */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* destination is a view into the original buffer */
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.buffer == old_buffer.buffer + old_buffer.len);
        assert(output.allocator == old_buffer.allocator);
    } else {
        /* source buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* destination unchanged */
        assert(output.buffer == old_output.buffer);
        assert(output.len == old_output.len);
        assert(output.capacity == old_output.capacity);
        assert(output.allocator == old_output.allocator);
    }

    /* 7. Invariants that must always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
