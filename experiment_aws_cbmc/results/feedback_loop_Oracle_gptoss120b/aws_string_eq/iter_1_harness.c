#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/array.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 16

void aws_string_eq_harness(void) {
    /* nondet strings */
    struct aws_string *str_a = NULL;
    struct aws_string *str_b = NULL;

    /* allocate first string (may stay NULL) */
    if (__CPROVER_nondet_uint()) {
        size_t len_a = __CPROVER_nondet_uint();
        __CPROVER_assume(len_a <= MAX_LEN);
        str_a = malloc(sizeof(struct aws_string) + (len_a ? len_a - 1 : 0));
        __CPROVER_assume(str_a != NULL);

        /* set const fields via casts */
        ((struct aws_allocator * const *)(&str_a->allocator))[0] = NULL;
        ((size_t const *)(&str_a->len))[0] = len_a;

        /* nondet bytes */
        for (size_t i = 0; i < len_a; ++i) {
            ((uint8_t *)str_a->bytes)[i] = __CPROVER_nondet_uchar();
        }
    }

    /* allocate second string (may stay NULL) */
    if (__CPROVER_nondet_uint()) {
        size_t len_b = __CPROVER_nondet_uint();
        __CPROVER_assume(len_b <= MAX_LEN);
        str_b = malloc(sizeof(struct aws_string) + (len_b ? len_b - 1 : 0));
        __CPROVER_assume(str_b != NULL);

        ((struct aws_allocator * const *)(&str_b->allocator))[0] = NULL;
        ((size_t const *)(&str_b->len))[0] = len_b;

        for (size_t i = 0; i < len_b; ++i) {
            ((uint8_t *)str_b->bytes)[i] = __CPROVER_nondet_uchar();
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str_a != NULL, aws_string_is_valid(str_a)));
    __CPROVER_assume(IMPLIES(str_b != NULL, aws_string_is_valid(str_b)));

    /* snapshot pre‑state for frame condition */
    size_t len_a_before = 0, len_b_before = 0;
    uint8_t *bytes_a_before = NULL, *bytes_b_before = NULL;

    if (str_a) {
        len_a_before = str_a->len;
        if (len_a_before) {
            bytes_a_before = malloc(len_a_before);
            __CPROVER_assume(bytes_a_before != NULL);
            memcpy(bytes_a_before, str_a->bytes, len_a_before);
        }
    }
    if (str_b) {
        len_b_before = str_b->len;
        if (len_b_before) {
            bytes_b_before = malloc(len_b_before);
            __CPROVER_assume(bytes_b_before != NULL);
            memcpy(bytes_b_before, str_b->bytes, len_b_before);
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq(str_a, str_b);

    /* post‑conditions */

    /* 1. return‑value correctness */
    if (str_a == str_b) {
        assert(result == true);
    } else if (str_a == NULL || str_b == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str_a->bytes, str_a->len,
                                     str_b->bytes, str_b->len);
        assert(result == expected);
    }

    /* 2. frame condition – strings must be unchanged */
    if (str_a) {
        assert(str_a->len == len_a_before);
        if (len_a_before) {
            assert(memcmp(str_a->bytes, bytes_a_before, len_a_before) == 0);
        }
    }
    if (str_b) {
        assert(str_b->len == len_b_before);
        if (len_b_before) {
            assert(memcmp(str_b->bytes, bytes_b_before, len_b_before) == 0);
        }
    }

    /* clean up */
    free(bytes_a_before);
    free(bytes_b_before);
    free(str_a);
    free(str_b);
    return 0;
}
