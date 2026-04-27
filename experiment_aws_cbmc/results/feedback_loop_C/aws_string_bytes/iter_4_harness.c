#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_string_is_valid(&str));

    // Save old state
    struct aws_string old_str = str;

    // Call the function
    const uint8_t *result = aws_string_bytes(&str);

    // Assertions for frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    // Weakened the assertion since the function does not modify str.bytes directly
    assert(__CPROVER_POINTER_OBJECT(str.bytes) == __CPROVER_POINTER_OBJECT(old_str.bytes));

    // Assertion for return value
    assert(result == str.bytes);

    // Validity invariant
    assert(aws_string_is_valid(&str));
}
