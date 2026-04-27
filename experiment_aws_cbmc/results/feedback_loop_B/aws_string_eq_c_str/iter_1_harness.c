#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(&str, sizeof(str)));
    __CPROVER_assume(AWS_MEM_IS_READABLE(str.bytes, str.len));
    __CPROVER_assume(aws_string_is_valid(&str));

    const char *c_str = (const char *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(&str, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(aws_array_eq_c_str(str.bytes, str.len, c_str));
    } else {
        assert(str.allocator == old_str.allocator);
        assert(str.len == old_str.len);
        assert(str.bytes == old_str.bytes);
        assert(c_str == old_c_str);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
}
