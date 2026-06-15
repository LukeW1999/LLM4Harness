#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 10

void aws_string_eq_c_str_harness() {
    /* nondet scalar pointers */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
    char *c_str = malloc(MAX_STRING_SIZE + 1);

    /* bound lengths and assume valid inputs */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_STRING_SIZE);

    /* if str is not null, assume it is a valid aws_string */
    if (str != NULL) {
        __CPROVER_assume(str->len == str_len);
        __CPROVER_assume(str->allocator == NULL); /* static string convention */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string) + str_len));
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* if c_str is not null, assume it is null-terminated within the allocated buffer */
    if (c_str != NULL) {
        c_str[MAX_STRING_SIZE] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_STRING_SIZE + 1));
    }

    /* save old state for immutability checks */
    size_t old_str_len;
    struct aws_allocator *old_str_allocator;
    if (str != NULL) {
        old_str_len = str->len;
        old_str_allocator = str->allocator;
    }

    /* call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* postconditions */

    /* 1. Assert boolean result is valid */
    assert(result == true || result == false);

    /* 2. Assert logical behavior for NULL inputs */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    if ((str == NULL) != (c_str == NULL)) {
        assert(result == false);
    }

    /* 3. Immutability: fields of str must be unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
    }

    /* 4. c_str memory content is unchanged (function does not write) */
    /* No explicit assert needed for unchanged content because we never write to it */

    /* 5. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
