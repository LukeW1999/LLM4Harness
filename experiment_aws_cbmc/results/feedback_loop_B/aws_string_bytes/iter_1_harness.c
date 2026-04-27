#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(str.bytes, str.len));
    __CPROVER_assume(str.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str.allocator != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function always succeeds and returns the bytes field, so we only need to check the success case */
    assert(result == str.bytes);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old.allocator);
    assert(str.len == old.len);

    /* 6. Assert validity invariant always holds */
    assert(AWS_MEM_IS_READABLE(result, str.len));
}
