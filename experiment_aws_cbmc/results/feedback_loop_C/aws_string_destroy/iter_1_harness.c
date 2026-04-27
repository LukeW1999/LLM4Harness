// === STEP 1: SUCCESS PATH ===
// When aws_string_destroy returns AWS_OP_SUCCESS (or the successful value):
//   - str: FREES the memory if str->allocator is not NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_string_destroy returns AWS_OP_ERR (or fails):
//   - str: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   str (struct aws_string*):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(old_str): YES (must hold before call)
//   - aws_string_is_valid(str): NO (memory is freed)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string) + sizeof(uint8_t));
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_string old_str = *str;

    aws_string_destroy(str);

    // Check frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes[0] == old_str.bytes[0]);

    // Check validity invariants
    assert(!aws_string_is_valid(str));
}
