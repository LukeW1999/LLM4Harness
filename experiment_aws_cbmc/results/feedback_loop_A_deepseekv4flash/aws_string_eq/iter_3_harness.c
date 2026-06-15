#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (!a_is_null) {
        a = malloc(sizeof(struct aws_string) + 10);
        __CPROVER_assume(a != NULL);
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= 10);
        a->bytes = (uint8_t *)(a + 1);
        for (size_t i = 0; i < a->len; i++) {
            a->bytes[i] = nondet_uint8_t();
        }
        a->bytes[a->len] = '\0';
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        b = malloc(sizeof(struct aws_string) + 10);
        __CPROVER_assume(b != NULL);
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= 10);
        b->bytes = (uint8_t *)(b + 1);
        for (size_t i = 0; i < b->len; i++) {
            b->bytes[i] = nondet_uint8_t();
        }
        b->bytes[b->len] = '\0';
        __CPROVER_assume(aws_string_is_valid(b));
    }

    bool result = aws_string_eq(a, b);

    if (a == NULL && b == NULL) {
        __CPROVER_assert(result == true, "both NULL => true");
    } else if (a == NULL || b == NULL) {
        __CPROVER_assert(result == false, "one NULL => false");
    } else {
        if (a->len != b->len) {
            __CPROVER_assert(result == false, "different lengths => false");
        } else {
            bool all_equal = true;
            for (size_t i = 0; i < a->len; i++) {
                if (a->bytes[i] != b->bytes[i]) {
                    all_equal = false;
                    break;
                }
            }
            if (all_equal) {
                __CPROVER_assert(result == true, "same content => true");
            } else {
                __CPROVER_assert(result == false, "different content => false");
            }
        }
        __CPROVER_assert(aws_string_is_valid(a), "a is valid");
        __CPROVER_assert(aws_string_is_valid(b), "b is valid");
    }
}
