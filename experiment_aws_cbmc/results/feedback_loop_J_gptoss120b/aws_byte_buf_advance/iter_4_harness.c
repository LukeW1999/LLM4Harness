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
    struct aws_byte_buf buffer;
    struct aws_byte_buf output = {0};
    size_t len = nondet_size_t();

    /* buffer must be a well‑formed, bounded byte buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* additional preconditions required by aws_byte_buf_advance */
    __CPROVER_assume(len <= buffer.len);          /* advance can succeed */
    __CPROVER_assume(buffer.buffer != NULL);     /* pointer arithmetic safe */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);    /* keep within our model limits */

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* Success path – buffer should be advanced and output should describe the slice */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == old_buffer.buffer);
        assert(output.capacity == len);
        assert(output.len == len);
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
