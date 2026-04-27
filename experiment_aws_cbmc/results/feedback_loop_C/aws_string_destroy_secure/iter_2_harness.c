#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;
    struct aws_string old_str;

    // Initialize str with non-deterministic values
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Save the old state of the string
    old_str.allocator = str.allocator;
    old_str.len = str.len;
    old_str.bytes = str.bytes;

    // Save the old state of the bytes
    uint8_t *old_bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < str.len; i++) {
        old_bytes[i] = str.bytes[i];
    }

    // Call the function
    int result = aws_string_destroy_secure(&str);

    // Check frame conditions and validity invariants
    assert(str.allocator == old_str.allocator); // allocator is unchanged
    assert(str.len == old_str.len); // len is unchanged

    if (result == AWS_OP_SUCCESS) {
        // If operation was successful, bytes should be zeroed out
        for (size_t i = 0; i < str.len; i++) {
            assert(str.bytes[i] == 0);
        }
    } else {
        // If operation failed, bytes should remain unchanged
        for (size_t i = 0; i < str.len; i++) {
            assert(str.bytes[i] == old_bytes[i]);
        }
    }

    // Check validity invariant
    assert(aws_string_is_valid(&str) || result == AWS_OP_ERR);

    free(old_bytes);
}
