#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 2

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* Non-deterministically create or not each string */
    if (nondet_bool()) {
        a = (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(a != NULL);
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_STRING_LEN);
        a->bytes[a->len] = 0;
        for (size_t i = 0; i < a->len; i++) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (nondet_bool()) {
        b = (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(b != NULL);
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_STRING_LEN);
        b->bytes[b->len] = 0;
        for (size_t i = 0; i < b->len; i++) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old bytes for immutability checks */
    uint8_t old_a_bytes[MAX_STRING_LEN];
    uint8_t old_b_bytes[MAX_STRING_LEN];
    if (a != NULL) {
        for (size_t i = 0; i < a->len; i++) {
            old_a_bytes[i] = a->bytes[i];
        }
    }
    if (b != NULL) {
        for (size_t i = 0; i < b->len; i++) {
            old_b_bytes[i] = b->bytes[i];
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Assert postcondition: correctness of the returned value */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (a->len == b->len) {
            bool bytes_equal = true;
            for (size_t i = 0; i < a->len; i++) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_equal = false;
                    break;
                }
            }
            assert(result == bytes_equal);
        } else {
            assert(result == false);
        }
    }

    /* Assert immutability: input strings were not modified */
    if (a != NULL) {
        for (size_t i = 0; i < a->len; i++) {
            assert(a->bytes[i] == old_a_bytes[i]);
        }
    }
    if (b != NULL) {
        for (size_t i = 0; i < b->len; i++) {
            assert(b->bytes[i] == old_b_bytes[i]);
        }
    }

    /* Assert validity invariants hold after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* Clean up */
    free(a);
    free(b);
}
