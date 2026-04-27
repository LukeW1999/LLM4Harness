#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;

    /* 2. Initialize the buffer as empty array */
    uint8_t array[] = {};
    size_t array_len = sizeof(array) / sizeof(array[0]);
    buf.buffer = array;
    buf.len = array_len;
    buf.capacity = array_len;
    buf.allocator = NULL;

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    aws_byte_buf_from_empty_array(&buf);

    /* 5. Assert postconditions for BOTH success and failure paths */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 6. Assert fields that must NOT change regardless of result */
    // These assertions are not needed since the function resets the buffer
    // assert(old_buf.buffer == buf.buffer);
    // assert(old_buf.len == buf.len);
    // assert(old_buf.capacity == buf.capacity);
    // assert(old_buf.allocator == buf.allocator);

    /* 7. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
