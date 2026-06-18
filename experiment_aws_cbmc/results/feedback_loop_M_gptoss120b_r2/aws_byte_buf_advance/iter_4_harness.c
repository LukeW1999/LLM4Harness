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
    /* 1. Declare and bound the buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    buffer.allocator = aws_default_allocator();
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(buffer.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Declare and initialize the output buffer as an empty valid buffer */
    struct aws_byte_buf output;
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 3. Nondeterministic length, bounded and fitting in the current length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.len);
    /* If we are going to advance a non‑zero length, the source buffer must be non‑NULL */
    __CPROVER_assume(len == 0 || buffer.buffer != NULL);

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Post‑condition checks */
    if (result) {
        /* success: buffer advanced */
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.len <= buffer.capacity);

        /* output describes the advanced region */
        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        /* failure: output zeroed, buffer unchanged */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);
    }

    /* 7. Invariant: both structures remain valid */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
