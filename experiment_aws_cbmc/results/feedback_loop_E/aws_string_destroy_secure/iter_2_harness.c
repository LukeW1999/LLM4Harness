#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    /* parameters */
    struct aws_string *str;
    struct aws_allocator *allocator;

    /* assumptions */
    __CPROVER_assume(AWS_MEM_IS_READABLE(&allocator, sizeof(struct aws_allocator)));
    __CPROVER_assume(allocator != NULL);

    /* allocate and initialize str */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);
    str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    str->allocator = allocator;
    str->len = len;
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0; // null terminator

    /* save old state */
    struct aws_string old = *str;

    /* operation under verification */
    aws_string_destroy_secure(str);

    /* assertions */
    // Check that the data bytes were zeroed out
    for (size_t i = 0; i < old.len; i++) {
        assert(old.bytes[i] == 0);
    }

    // Since the string is destroyed, we cannot directly access str->bytes or str->len
    // Instead, we can assert that the allocator's deallocate function was called
    // For this, we need to mock the allocator and track deallocation calls
    // However, without specific knowledge of the allocator's implementation, we can only assume
    // that the allocator's deallocate function was called with the correct pointer

    // Assuming a simple mock allocator with a global counter for deallocations
    extern size_t deallocation_count;
    extern void *last_deallocated_ptr;

    // Reset the deallocation count and last deallocated pointer before the operation
    deallocation_count = 0;
    last_deallocated_ptr = NULL;

    // Call the function again to simulate the deallocation
    aws_string_destroy_secure(str);

    // Assert that the deallocation count is 1 and the last deallocated pointer is the original str
    assert(deallocation_count == 1);
    assert(last_deallocated_ptr == (void *)str);
}
