// === STEP 1: SUCCESS PATH ===
// When aws_string_destroy_secure returns AWS_OP_SUCCESS (or the successful value):
//   - str->bytes: CHANGES to 0 (zeroed out)
//
// === STEP 2: FAILURE PATH ===
// When aws_string_destroy_secure returns AWS_OP_ERR (or fails):
//   - str->allocator: UNCHANGED
//   - str->len: UNCHANGED
//   - str->bytes: UNCHANGED
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
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;
    struct aws_string old_str = str;

    // Initialize str with non-deterministic values
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Save the old state of the bytes
    uint8_t *old_bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < str.len; i++) {
        old_bytes[i] = str.bytes[i];
    }

    aws_string_destroy_secure(&str);

    // Check frame conditions and validity invariants
    if (old_str.allocator != NULL) {
        assert(str.allocator == old_str.allocator); // allocator is unchanged
    }
    assert(str.len == old_str.len); // len is unchanged

    if (old_str.allocator != NULL) {
        // If allocator is not NULL, bytes should be zeroed out
        for (size_t i = 0; i < str.len; i++) {
            assert(str.bytes[i] == 0);
        }
    } else {
        // If allocator is NULL, bytes should remain unchanged
        for (size_t i = 0; i < str.len; i++) {
            assert(str.bytes[i] == old_bytes[i]);
        }
    }

    // Check validity invariant
    assert(aws_string_is_valid(&str));

    free(old_bytes);
}
