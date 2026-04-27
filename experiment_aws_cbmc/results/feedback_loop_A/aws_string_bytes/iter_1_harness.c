#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string s;
    __CPROVER_assume(AWS_MEM_IS_READABLE(&s, sizeof(struct aws_string)));
    __CPROVER_assume(AWS_MEM_IS_READABLE(s.bytes, s.len));
    __CPROVER_assume(s.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_string_is_valid(&s));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = s;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(&s);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since aws_string_bytes is a simple accessor, it always succeeds */
    assert(result == s.bytes);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(s.allocator == old.allocator);
    assert(s.len == old.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&s));
}
