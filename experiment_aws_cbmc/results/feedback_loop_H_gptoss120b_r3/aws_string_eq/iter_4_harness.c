#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* nondeterministic allocation for string a */
    struct aws_string *a = NULL;
    if (nondet_bool()) {
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } a_obj;
        a = &a_obj.s;
        a->allocator = aws_default_allocator();
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_STRING_LEN);
        for (size_t i = 0; i < a->len; ++i) {
            a_obj.bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* nondeterministic allocation for string b */
    struct aws_string *b = NULL;
    if (nondet_bool()) {
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } b_obj;
        b = &b_obj.s;
        b->allocator = aws_default_allocator();
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_STRING_LEN);
        for (size_t i = 0; i < b->len; ++i) {
            b_obj.bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* possibly make a and b point to the same object */
    if (a && b && nondet_bool()) {
        b = a;
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */
    if (result) {
        if (a != b) {
            assert(a != NULL);
            assert(b != NULL);
            assert(a->len == b->len);
            for (size_t i = 0; i < a->len; ++i) {
                assert(a->bytes[i] == b->bytes[i]);
            }
        }
    } else {
        if (a == NULL || b == NULL) {
            /* If one is NULL and the other is not, result must be false */
            assert(!(a == NULL && b == NULL));
        } else {
            /* Both non‑NULL and strings differ */
            bool differ = false;
            if (a->len != b->len) {
                differ = true;
            } else {
                for (size_t i = 0; i < a->len; ++i) {
                    if (a->bytes[i] != b->bytes[i]) {
                        differ = true;
                        break;
                    }
                }
            }
            assert(differ);
        }
    }

    /* Validity invariants */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
