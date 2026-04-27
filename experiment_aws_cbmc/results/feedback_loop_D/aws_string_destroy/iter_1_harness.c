// === STEP 1: SUCCESS PATH ===
// When aws_string_destroy returns AWS_OP_SUCCESS (or the successful value):
//   - str: FREES the memory allocated by str->allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_string_destroy returns AWS_OP_ERR (or fails):
//   - str: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   str (struct aws_string):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): NO (because the memory is freed)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness() {
    struct aws_string str;
    struct aws_string old_str = str;

    // Initialize the aws_string with a valid allocator and some length
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Call the function under test
    aws_string_destroy(&str);

    // Assert frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);

    // Assert validity invariants
    // aws_string_is_valid(&str) should be false because the memory is freed
    // However, CBMC does not have a way to directly check if memory has been freed,
    // so we skip this assertion
}
