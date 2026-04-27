#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_buffer = buffer;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(buffer.len == old_buffer.len + len);
        assert(output.buffer == (old_buffer.buffer == NULL ? NULL : old_buffer.buffer + old_buffer.len));
        assert(output.len == 0);
        assert(output.capacity == len);
    } else {
        assert(buffer.len == old_buffer.len);
        assert(AWS_MEM_IS_UNINITIALIZED(output.buffer, len));
        assert(output.len == 0);
        assert(output.capacity == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.buffer == old_buffer.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
