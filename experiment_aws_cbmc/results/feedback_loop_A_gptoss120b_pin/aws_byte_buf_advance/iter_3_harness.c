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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Ensure the two structs are distinct to avoid aliasing issues */
    __CPROVER_assume(&output != &buffer);

    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions */
    if (result) {
        /* Success: buffer advanced */
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* output points to the original start of the buffer */
        assert(output.buffer == old_buffer.buffer);
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == NULL);
    } else {
        /* Failure: buffer unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* output zeroed */
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.len == 0);
        assert(output.allocator == NULL);
    }

    /* 5. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
