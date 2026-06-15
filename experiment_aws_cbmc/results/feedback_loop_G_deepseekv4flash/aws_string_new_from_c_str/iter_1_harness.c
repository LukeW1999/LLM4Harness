#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_new_from_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);
    char *c_str = (char *)malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[len] = '\0';

    /* State before call: none to save since function creates a new object */

    /* Call the function */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postconditions */
    if (result != NULL) {
        /* Success path */
        assert(aws_string_is_valid(result));
        assert(result->len == len);
        assert(result->allocator == allocator);
        /* Verify bytes match */
        for (size_t i = 0; i < len; i++) {
            assert(result->bytes[i] == (uint8_t)c_str[i]);
        }
        assert(result->bytes[len] == '\0');
        /* Memory must be accessible for the entire string */
        assert(AWS_MEM_IS_READABLE(result->bytes, len + 1));
    } else {
        /* Failure path: nothing to assert about the result, but ensure allocator wasn't violated */
        /* (Allocator may be NULL or allocation failed) */
    }

    /* Cleanup */
    free((void *)c_str);
    if (allocator != NULL) {
        free(allocator);
    }
}
