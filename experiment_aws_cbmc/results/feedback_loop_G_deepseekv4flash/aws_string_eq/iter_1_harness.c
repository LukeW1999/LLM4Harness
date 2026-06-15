#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

/* Bounding constant for string length */
#define MAX_STRING_LEN 10

void aws_string_eq_harness() {
    /* Non-deterministic valid strings */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (nondet_bool()) {
        /* a is a valid dynamically allocated string */
        size_t len_a;
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + len_a);
        if (a != NULL) {
            /* Because fields are const, we simulate via a cast to non-const for init */
            struct aws_string *a_mutable = a;
            a_mutable->allocator = NULL; /* or nondet, but valid strings can have NULL iff static */
            a_mutable->len = len_a;
            /* bytes are non-deterministic, but ensure null terminator at position len_a */
            __CPROVER_assume(AWS_MEM_IS_READABLE(a->bytes, len_a + 1));
            /* Assume validity predicate holds */
            __CPROVER_assume(aws_string_is_valid(a));
        }
    }
    if (nondet_bool()) {
        /* b is a valid dynamically allocated string */
        size_t len_b;
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + len_b);
        if (b != NULL) {
            struct aws_string *b_mutable = b;
            b_mutable->allocator = NULL;
            b_mutable->len = len_b;
            __CPROVER_assume(AWS_MEM_IS_READABLE(b->bytes, len_b + 1));
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Ensure that if strings are non-NULL, they satisfy memory constraints */
    if (a != NULL) {
        __CPROVER_assume(aws_string_is_valid(a));
        __CPROVER_assume(AWS_MEM_IS_READABLE(a->bytes, a->len + 1));
    }
    if (b != NULL) {
        __CPROVER_assume(aws_string_is_valid(b));
        __CPROVER_assume(AWS_MEM_IS_READABLE(b->bytes, b->len + 1));
    }

    /* Call the function */
    bool result = aws_string_eq(a, b);

    /* Postconditions */
    if (a == b) {
        /* Same pointer (including both NULL) -> true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* Exactly one is NULL -> false */
        assert(result == false);
    } else {
        /* Both non-NULL and different pointers */
        assert(a->len == a->len); /* tautology, but necessary for boundedness */
        if (a->len != b->len) {
            assert(result == false);
        } else {
            /* Arrays are same length; result should be true iff bytes match */
            bool bytes_match = true;
            for (size_t i = 0; i < a->len; i++) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_match = false;
                    break;
                }
            }
            assert(result == bytes_match);
        }
    }

    /* Immutability: strings remain valid */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
