#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_string_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(aws_string_is_bounded(&str, MAX_BUFFER_SIZE));
    ensure_string_has_allocated_buffer_member(&str);
    __CPROVER_assume(aws_string_is_valid(&str));

    const char *c_str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str.len + 1)); // Null-terminated

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(&str, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == true) {
        assert_bytes_match(str.bytes, (const uint8_t *)c_str, str.len);
    } else {
        // No specific changes to assert on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old.allocator);
    assert(str.len == old.len);
    assert(str.bytes == old.bytes);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
}
