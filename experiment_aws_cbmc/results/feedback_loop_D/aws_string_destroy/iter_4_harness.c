#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string));
    assume(str != NULL);

    // Initialize the aws_string with a valid allocator and some length
    str->allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    assume(str->allocator != NULL);
    str->len = nondet_size_t();
    str->bytes = (const uint8_t *)malloc(str->len + 1); // +1 for null terminator
    assume(str->bytes != NULL);

    // Call the function under test
    aws_string_destroy(str);

    // After destruction, the string structure should be in a valid state.
    // Since the actual memory deallocation is not tracked by CBMC, we can't assert
    // that the memory has been freed, but we can assert that the structure is reset.
    assert(str->allocator == NULL);
    assert(str->bytes == NULL);
    assert(str->len == 0);

    // Free the string structure itself
    free(str);
}
