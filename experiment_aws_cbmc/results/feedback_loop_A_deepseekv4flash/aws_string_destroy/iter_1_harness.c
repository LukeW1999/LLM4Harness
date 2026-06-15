#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string));
    if (str != NULL) {
        // Nondeterministically set allocator to NULL or a valid pointer
        str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        // Set remaining fields; validity will be assumed.
        // Note: len and bytes are const, but we can still assign them for non-const?
        // In C, these are const fields, so we cannot assign them directly.
        // However, aws_string_is_valid will check fields as const; we must initialize them before calling is_valid.
        // The proper way is to allocate the string with flexible array member, but for simplicity,
        // we can use a non-const cast or use the AWS_STATIC_STRING_FROM_LITERAL approach.
        // Actually, aws_string_is_valid reads only allocator, len, and bytes[0] if len>0.
        // Since we only care about allocator for the destroy function, we can set other fields arbitrarily.
        // But to be safe, we'll use __CPROVER_assume to set them via struct pointer.
        str->len = nondet_size_t();
        // bytes is a flexible array; we only need to ensure that if len>0, bytes[0] is accessible.
        // For cbmc, we can assume that the memory for bytes exists.
        // Simpler: assume the string is created via a constructor, but we don't have those here.
        // We'll just assume the string is valid, meaning the struct is properly initialized.
        // Since we cannot assign to const fields, we will rely on the assumption that the struct was created correctly.
    }

    // Assume that if str is non-NULL, it is a valid aws_string
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    // Capture preconditions for later assertions
    bool will_free = (str != NULL) && (str->allocator != NULL);

    // Call the function under test
    aws_string_destroy(str);

    // Postcondition assertions
    if (str == NULL) {
        // No-op: nothing to assert
    } else {
        if (!will_free) {
            // allocator was NULL, so str was not freed; it should still be valid
            assert(aws_string_is_valid(str));
        } else {
            // str was freed; accessing it is undefined, so no assertions
        }
    }
}
