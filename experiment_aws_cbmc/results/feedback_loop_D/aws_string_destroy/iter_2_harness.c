#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness() {
    struct aws_string str;
    struct aws_string old_str;

    // Initialize the aws_string with a valid allocator and some length
    str.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    assume(str.allocator != NULL);
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)malloc(str.len + 1); // +1 for null terminator
    assume(str.bytes != NULL);

    // Make a copy of the original string for later comparison
    old_str = str;

    // Call the function under test
    aws_string_destroy(&str);

    // Assert frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);

    // Free the allocator and bytes manually since CBMC cannot track memory deallocation
    free((void *)str.allocator);
    free((void *)old_str.bytes);
}
