// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_advance returns true:
//   - buffer.len: CHANGES to buffer.len + len
//   - output.buffer: CHANGES to buffer.buffer + buffer.len (old buffer.len)
//   - output.len: CHANGES to 0
//   - output.capacity: CHANGES to len
//   - output.allocator: CHANGES to NULL

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_advance returns false:
//   - buffer.len: UNCHANGED
//   - output.buffer: UNCHANGED
//   - output.len: UNCHANGED
//   - output.capacity: UNCHANGED
//   - output.allocator: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// buffer (struct aws_byte_buf):
//   - len: CHANGED on success, UNCHANGED on failure
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// output (struct aws_byte_buf):
//   - buffer: CHANGED on success, UNCHANGED on failure
//   - len: CHANGED on success, UNCHANGED on failure
//   - capacity: CHANGED on success, UNCHANGED on failure
//   - allocator: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buffer): YES (must hold after call)
//   - aws_byte_buf_is_valid(&output): YES on success, NO on failure

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    // Initialize buffer and output with arbitrary values
    __CPROVER_assume(len <= buffer.capacity); // Ensure len does not exceed capacity
    ensure_byte_buf_has_allocated_buffer_member(&buffer);

    // Save old state
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    // Frame conditions and validity invariants
    if (result) {
        // Success path assertions
        assert(buffer.len == old_buffer.len + len);
        assert(output.buffer == old_buffer.buffer + old_buffer.len);
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        // Failure path assertions
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(output.buffer == old_output.buffer);
        assert(output.len == old_output.len);
        assert(output.capacity == old_output.capacity);
        assert(output.allocator == old_output.allocator);
        assert(aws_byte_buf_is_valid(&buffer));
        // output is not guaranteed to be valid on failure
    }
}
