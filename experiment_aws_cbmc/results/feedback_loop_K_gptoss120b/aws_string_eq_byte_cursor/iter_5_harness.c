#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* Preconditions */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(cur == NULL || (cur->ptr == NULL ? cur->len == 0 : cur->len >= 0));

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition: return value correctness */
    if (str == NULL && cur == NULL) {
        __CPROVER_assert(result == true, "Result should be true when both arguments are NULL");
    } else if (str == NULL || cur == NULL) {
        __CPROVER_assert(result == false, "Result should be false when exactly one argument is NULL");
    } else {
        bool eq = (str->len == cur->len) &&
                  (str->len == 0 || memcmp(str->bytes, cur->ptr, str->len) == 0);
        __CPROVER_assert(result == eq, "Result should reflect byte-wise equality");
    }
}
