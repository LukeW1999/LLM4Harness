// === STEP 1: SUCCESS PATH ===
// When aws_string_destroy_secure returns AWS_OP_SUCCESS (or the successful value):
//   - str->bytes: CHANGES to all zeros
//   - str->allocator: UNCHANGED
//   - str->len: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_string_destroy_secure returns AWS_OP_ERR (or fails):
//   - str->bytes: UNCHANGED
//   - str->allocator: UNCHANGED
//   - str->len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   str (struct aws_string):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness() {
    struct aws_string str;
    struct aws_string old_str = str;

    // Initialize the aws_string structure with non-deterministic values
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Save the original bytes for comparison
    uint8_t *original_bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(original_bytes != NULL);
    for (size_t i = 0; i < str.len; i++) {
        original_bytes[i] = str.bytes[i];
    }

    aws_string_destroy_secure(&str);

    // Assert frame conditions and validity invariants
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    if (old_str.bytes != NULL) {
        if (old_str.allocator != NULL) {
            // If allocator is not null, bytes should be zeroed
            for (size_t i = 0; i < old_str.len; i++) {
                assert(str.bytes[i] == 0);
            }
        } else {
            // If allocator is null, bytes should remain unchanged
            for (size_t i = 0; i < old_str.len; i++) {
                assert(str.bytes[i] == original_bytes[i]);
            }
        }
    }
    assert(aws_string_is_valid(&str));

    free(original_bytes);
}
