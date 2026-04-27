#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;

    /* 2. Initialize the byte buffer with an empty array */
    aws_byte_buf_from_empty_array(&buf);

    /* 3. Assert postconditions (infer from what the function guarantees) */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
}
