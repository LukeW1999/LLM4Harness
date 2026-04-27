#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_destroy_secure_harness() {
    /* parameters */
    struct aws_string *str;
    struct aws_allocator *allocator;

    /* assumptions */
    __CPROVER_assume(AWS_MEM_IS_READABLE(&str, sizeof(struct aws_string)));
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
    if (str) {
        // Check that the data bytes were zeroed out
        for (size_t i = 0; i < old.len; i++) {
            assert(str->bytes[i] == 0);
        }
        // Check that the allocator was called to free the memory
        assert(str->allocator == old.allocator);
        // No other fields to check as the structure is destroyed
    }
    // No need to assert aws_string_is_valid(str) after destruction
}
