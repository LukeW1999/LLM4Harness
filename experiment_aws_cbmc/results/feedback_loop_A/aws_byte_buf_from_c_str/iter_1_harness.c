#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str = (const char *)any_ptr();
    size_t str_len = strlen(c_str);
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = aws_byte_buf_from_c_str(c_str);

    /* 3. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since aws_byte_buf_from_c_str does not return an error code, we only need to handle the success case */
    assert(buf.len == str_len);
    assert(buf.capacity == str_len);
    assert(buf.buffer == (uint8_t *)c_str);
    assert(buf.allocator == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as all fields are explicitly checked */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
