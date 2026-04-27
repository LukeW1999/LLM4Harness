#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    const char *c_str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, strlen(c_str) + 1));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For this function, there is no failure path as it does not modify the buffer */
    /* The function only compares the buffer with the C string and returns a boolean */
    /* Therefore, we only need to check the unchanged fields and the return value */

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.len == old.len);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
