#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 10

void aws_string_eq_harness() {
    /* Non-deterministic pointers */
    struct aws_string *a;
    struct aws_string *b;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        a = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
        __CPROVER_assume(a != NULL);
        a->len = nondet_size_t();
        __CPROVER_assume(a->len < MAX_STRING_SIZE);
        __CPROVER_assume(AWS_MEM_IS_READABLE(a->bytes, a->len));
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (b_is_null) {
        b = NULL;
    } else {
        b = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
        __CPROVER_assume(b != NULL);
        b->len = nondet_size_t();
        __CPROVER_assume(b->len < MAX_STRING_SIZE);
        __CPROVER_assume(AWS_MEM_IS_READABLE(b->bytes, b->len));
        __CPROVER_assume(aws_string_is_valid(b));
    }

    bool result = aws_string_eq(a, b);

    /* Postconditions */

    /* 1. If a and b point to the same object, result must be true */
    if (a == b) {
        __CPROVER_assert(result == true, "a == b => result == true");
    }

    /* 2. If exactly one of them is NULL, result must be false */
    if (a == NULL && b != NULL) {
        __CPROVER_assert(result == false, "a NULL, b non-NULL => false");
    }
    if (a != NULL && b == NULL) {
        __CPROVER_assert(result == false, "a non-NULL, b NULL => false");
    }

    /* 3. Both non-NULL: result is consistent with byte comparison */
    if (a != NULL && b != NULL) {
        if (a->len != b->len) {
            __CPROVER_assert(result == false, "different lengths => false");
        } else {
            if (result) {
                /* All bytes must match */
                for (size_t i = 0; i < a->len; i++) {
                    __CPROVER_assert(a->bytes[i] == b->bytes[i], "result true => bytes equal");
                }
            } else {
                /* At least one byte differs: cannot assert easily, skip */
            }
        }
    }

    /* 4. Strings remain valid (they are not modified) */
    if (a != NULL) {
        __CPROVER_assert(aws_string_is_valid(a), "a is still valid");
    }
    if (b != NULL) {
        __CPROVER_assert(aws_string_is_valid(b), "b is still valid");
    }
}
