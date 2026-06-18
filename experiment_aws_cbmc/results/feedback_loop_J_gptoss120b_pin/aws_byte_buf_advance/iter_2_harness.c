#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 2. Constrain len to a feasible advance (avoid overflow) */
    __CPROVER_assume(len <= buffer.capacity - buffer.len);
    __CPROVER_assume(buffer.buffer != NULL || len == 0);

    /* 3. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 4. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 5. Assert postconditions (result must be true under the above assumptions) */
    assert(result);

    /* Success: buffer advanced */
    assert(buffer.len == old_buffer.len + len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.buffer == old_buffer.buffer);
    assert(buffer.allocator == old_buffer.allocator);

    /* Success: output initialized */
    uint8_t *expected_ptr = (old_buffer.buffer == NULL) ? NULL : old_buffer.buffer + old_buffer.len;
    assert(output.buffer == expected_ptr);
    assert(output.capacity == len);
    assert(output.len == 0);
    assert(output.allocator == NULL);

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
