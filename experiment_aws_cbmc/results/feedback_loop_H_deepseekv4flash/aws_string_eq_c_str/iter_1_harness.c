#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_c_str_harness() {
    /* Non-deterministic allocation and initialization of str */
    struct aws_string *str = nondet_bool() ? malloc(sizeof(struct aws_string) + MAX_STRING_SIZE) : NULL;
    if (str) {
        str->allocator = NULL; /* Simulate a static or dynamically allocated string */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_SIZE);
        str->len = len;
        for (size_t i = 0; i < len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic allocation and initialization of c_str */
    char *c_str = nondet_bool() ? malloc(MAX_STRING_SIZE) : NULL;
    if (c_str) {
        size_t c_len;
        __CPROVER_assume(c_len < MAX_STRING_SIZE);
        for (size_t i = 0; i < c_len; i++) {
            c_str[i] = nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* Save old state if str is non-NULL */
    struct aws_string old_str;
    if (str) {
        old_str = *str;
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions based on NULL combinations */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }

    /* Unchanged fields and validity invariants for str */
    if (str) {
        assert(str->len == old_str.len);
        assert(str->allocator == old_str.allocator);
        assert(aws_string_is_valid(str));
    }
}
