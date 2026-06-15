#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer = {0};
    struct aws_byte_buf output = {0};
    size_t len = nondet_size_t();

    /* buffer must be a well‑formed, bounded byte buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    buffer.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* set a concrete length for the buffer within its capacity */
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);

    /* additional preconditions for the harness */
    __CPROVER_assume(buffer.buffer != NULL);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(len <= buffer.capacity);

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* Success path – buffer should be advanced and output should describe the slice */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        __CPROVER_assert(
            (uintptr_t)buffer.buffer == (uintptr_t)old_buffer.buffer + len,
            "buffer pointer advanced correctly");

        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        /* Failure path – buffer must be unchanged and output left empty */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    /* Both buffers must remain valid after the call */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
