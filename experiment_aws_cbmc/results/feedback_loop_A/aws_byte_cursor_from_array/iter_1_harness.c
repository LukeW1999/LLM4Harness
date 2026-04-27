#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness() {
    /* 1. Declare and bound data structures */
    const void *const bytes = nd_void_ptr();
    size_t len = nondet_size_t();
    __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions for BOTH success and failure paths */
    /* Since aws_byte_cursor_from_array is a simple assignment, it always succeeds */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 4. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as the function only initializes a new cursor */

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
