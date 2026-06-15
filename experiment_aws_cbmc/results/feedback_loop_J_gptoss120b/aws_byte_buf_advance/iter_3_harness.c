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
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len == 0 || buffer.buffer != NULL);
    __CPROVER_assume(len <= buffer.len); /* ensure advance can succeed */

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* Success path */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == old_buffer.buffer);
        assert(output.capacity == len);
        assert(output.len == len);
        assert(output.allocator == NULL);
    } else {
        /* Failure path – buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
