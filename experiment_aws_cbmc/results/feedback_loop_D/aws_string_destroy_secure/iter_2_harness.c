#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;
    struct aws_string old_str;

    // Initialize the aws_string structure with non-deterministic values
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)malloc(str.len);
    __CPROVER_assume(str.bytes != NULL);

    // Copy the original string for later comparison
    memcpy((void *)&old_str, (void *)&str, sizeof(struct aws_string));
    uint8_t *original_bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(original_bytes != NULL);
    memcpy(original_bytes, str.bytes, str.len);

    // Call the function under test
    int result = aws_string_destroy_secure(&str);

    // Assert frame conditions and validity invariants
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);

    if (result == AWS_OP_SUCCESS) {
        // On success, bytes should be zeroed
        for (size_t i = 0; i < old_str.len; i++) {
            assert(str.bytes[i] == 0);
        }
    } else {
        // On failure, bytes should remain unchanged
        for (size_t i = 0; i < old_str.len; i++) {
            assert(str.bytes[i] == original_bytes[i]);
        }
    }

    // Check if the string is still valid after destruction
    // Note: After destruction, the string is no longer valid, so this assertion should fail
    // assert(aws_string_is_valid(&str));

    free((void *)original_bytes);
}
