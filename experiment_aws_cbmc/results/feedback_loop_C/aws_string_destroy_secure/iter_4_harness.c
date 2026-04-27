#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;

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

    // Call the function
    aws_string_destroy_secure(&str);

    // Check frame conditions and validity invariants
    assert(str.allocator == NULL); // allocator is set to NULL
    assert(str.len == 0); // len is set to 0

    // If operation was successful, bytes should be zeroed out
    for (size_t i = 0; i < str.len; i++) {
        assert(str.bytes[i] == 0);
    }

    free(old_bytes);
}
