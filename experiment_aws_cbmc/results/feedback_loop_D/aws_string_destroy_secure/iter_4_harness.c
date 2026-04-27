#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;

    // Initialize the aws_string structure with non-deterministic values
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(str.bytes != NULL);

    // Create a copy of the original bytes for later comparison
    uint8_t *original_bytes = (uint8_t *)malloc(str.len);
    __CPROVER_assume(original_bytes != NULL);
    memcpy(original_bytes, str.bytes, str.len);

    // Call the function under test
    aws_string_destroy_secure(&str);

    // Assert that the bytes are zeroed out
    for (size_t i = 0; i < str.len; i++) {
        assert(str.bytes[i] == 0);
    }

    // Free the allocated memory for original_bytes
    free(original_bytes);
}
