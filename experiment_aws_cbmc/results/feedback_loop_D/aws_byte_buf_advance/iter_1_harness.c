// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_advance returns true:
//   - buffer.len: CHANGES to buffer.len + len
//   - output.buffer: CHANGES to buffer.buffer + buffer.len
//   - output.len: CHANGES to len
//   - output.capacity: CHANGES to len
//   - output.allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_advance returns false:
//   - buffer.len: UNCHANGED
//   - output.buffer: CHANGES to NULL
//   - output.len: CHANGES to 0
//   - output.capacity: CHANGES to 0
//   - output.allocator: CHANGES to NULL
//
// === STEP 3: FRAME CONDITIONS ===
// buffer (struct aws_byte_buf):
//   - len: CHANGED on success, UNCHANGED on failure
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// output (struct aws_byte_buf):
//   - len: CHANGED always
//   - buffer: CHANGED always
//   - capacity: CHANGED always
//   - allocator: CHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buffer): YES (must hold after call)
//   - aws_byte_buf_is_valid(&output): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    // Initialize buffer and output with non-deterministic values
    buffer.len = nondet_size_t();
    buffer.buffer = can_fail_malloc(buffer.len);
    buffer.capacity = nondet_size_t();
    buffer.allocator = (struct aws_allocator *)nondet_pointer();
    len = nondet_size_t();

    // Save old state of buffer
    struct aws_byte_buf old_buffer = buffer;

    // Call the function under test
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    // Assertions for success path
    if (result) {
        assert(buffer.len == old_buffer.len + len);
        assert(output.buffer == (old_buffer.buffer == NULL ? NULL : old_buffer.buffer + old_buffer.len));
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        // Assertions for failure path
        assert(buffer.len == old_buffer.len);
        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    // Free allocated memory
    free(buffer.buffer);
}
