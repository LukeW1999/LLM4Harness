// === STEP 1: SUCCESS PATH ===
// aws_string_bytes does not return a status code; it simply returns a pointer.
// Therefore, there is no success or failure path in terms of return codes.
// The function will always return str->bytes if str is not NULL.

// === STEP 2: FAILURE PATH ===
// Since there is no return code, there is no failure path in terms of return codes.
// However, if str is NULL, the function will dereference a NULL pointer, which is undefined behavior.
// For the purpose of this harness, we assume str is not NULL.

// === STEP 3: FRAME CONDITIONS ===
// For the struct aws_string parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_string type):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always (it's a pointer to the data, not the data itself)

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&param1): YES (must hold after call)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));

    // Save old state
    struct aws_string old_str = str;

    // Call the function
    const uint8_t *result = aws_string_bytes(&str);

    // Assertions for frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);

    // Assertion for return value
    assert(result == str.bytes);

    // Validity invariant
    assert(aws_string_is_valid(&str));
}
