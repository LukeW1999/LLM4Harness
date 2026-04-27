#include <aws/common/byte_buf.h>
#include <string.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_byte_cursor_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str = (const char *)any_memory();
    size_t max_size = MAX_BUFFER_SIZE;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, max_size));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function always succeeds, so we only need to check the success path */
    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == (cur.ptr) ? strlen(c_str) : 0);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as the function only initializes the cursor */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
