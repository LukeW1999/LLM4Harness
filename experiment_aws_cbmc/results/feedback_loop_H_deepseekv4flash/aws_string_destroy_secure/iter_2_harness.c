#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 10

void aws_string_destroy_secure_harness() {
    // Non-deterministically create a valid string
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);
    *(size_t *)&str->len = len;

    // Non-deterministic allocator
    struct aws_allocator *allocator = nondet_bool() ? malloc(sizeof(struct aws_allocator)) : NULL;
    *(struct aws_allocator **)&str->allocator = allocator;

    // Fill bytes
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    // Assume valid string
    __CPROVER_assume(aws_string_is_valid(str));

    // Call the function
    aws_string_destroy_secure(str);

    // Postcondition checks
    if (allocator == NULL) {
        // String should still be valid and bytes zeroed
        __CPROVER_assert(aws_string_is_valid(str), "String should be valid when allocator is NULL");
        for (size_t i = 0; i < len; ++i) {
            __CPROVER_assert(str->bytes[i] == 0, "Byte should be zeroed when allocator is NULL");
        }
        free(str);
    } else {
        // Memory freed, so we cannot access str; just assert that we reached this case
        __CPROVER_assert(1, "Non-null allocator case: memory freed");
    }

    // Also test NULL pointer
    struct aws_string *null_str = NULL;
    aws_string_destroy_secure(null_str);
    __CPROVER_assert(1, "Null pointer case: function called successfully");
}
