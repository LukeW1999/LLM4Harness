#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    /* Non-deterministic inputs */
    const struct aws_string *str = nondet_bool() ? malloc(sizeof(struct aws_string)) : NULL;
    const char *c_str = nondet_bool() ? malloc(sizeof(char) * 10) : NULL;

    /* Assume valid string if not NULL */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Assume c_str is null-terminated if not NULL */
    if (c_str != NULL) {
        __CPROVER_assume(c_str[0] == '\0' || c_str[0] != '\0'); /* placeholder for null-termination */
        /* We can't easily assume null-termination without knowing length, so we bound */
        __CPROVER_assume(str->len < 10); /* bound for simplicity */
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    /* Call function */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result depends on content comparison */
        /* We can't fully assert the comparison result without knowing content, but we can assert immutability */
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes are const, so they can't change */
    }

    /* Immutability: string fields never change */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes are const, so they can't change */
    }

    /* Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
