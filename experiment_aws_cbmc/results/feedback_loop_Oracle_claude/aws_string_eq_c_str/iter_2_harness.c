#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness(void) {
    /* Non-deterministic inputs */
    struct aws_string *str = nondet_bool() ? NULL : ensure_string_is_allocated_nondet_length();
    
    /* c_str can be NULL or a valid C string */
    const char *c_str;
    size_t c_str_len;
    __CPROVER_assume(c_str_len < 32); /* bound the length for tractability */
    char *c_str_buf = malloc(c_str_len + 1);
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        __CPROVER_assume(c_str_buf != NULL);
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    }

    /* Ground-truth preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* Save state for frame conditions */
    size_t old_len = (str != NULL) ? str->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition 1: If both are NULL, result must be true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: If exactly one is NULL, result must be false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Result is a valid boolean (always true for bool type, but explicit) */
    assert(result == true || result == false);

    /* Postcondition 4: Frame condition - str->len is not modified */
    if (str != NULL) {
        assert(str->len == old_len);
    }

    /* Postcondition 5: Frame condition - str validity is preserved */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    /* Postcondition 6: If result is true and both non-NULL, lengths must match */
    if (result == true && str != NULL && c_str != NULL) {
        /* The string length must equal strlen(c_str) */
        size_t computed_len = 0;
        const char *p = c_str;
        while (*p != '\0') {
            computed_len++;
            p++;
        }
        assert(str->len == computed_len);
    }
}
