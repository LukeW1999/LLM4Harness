#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_new_from_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_allocator *allocator = nondet_bool() ? NULL : can_fail_allocator();
    const char *c_str = nondet_bool() ? NULL : (const char *)malloc(sizeof(char) * (nondet_size_t() + 1));
    size_t len;

    /* Preconditions */
    if (c_str != NULL) {
        /* Ensure c_str is null-terminated and readable */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
        len = strlen(c_str);
    } else {
        len = 0;
    }

    /* Call function */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postconditions */
    if (result != NULL) {
        /* Success path */
        assert(aws_string_is_valid(result));
        assert(result->len == len);
        if (c_str != NULL) {
            assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);
        }
        /* Unchanged fields (implicit) */
        assert(result->allocator == allocator);
    } else {
        /* Failure path */
        assert(allocator == NULL || aws_allocator_is_valid(allocator) == false);
    }
}
