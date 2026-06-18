#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_harness(void) {
    struct aws_string *str_a = NULL;
    struct aws_string *str_b = NULL;
    const size_t MAX_LEN = 8;

    /* nondet allocation for str_a */
    if (__CPROVER_nondet_int()) {
        size_t len_a = __CPROVER_nondet_uint();
        __CPROVER_assume(len_a <= MAX_LEN);
        str_a = malloc(sizeof(struct aws_string) + len_a + 1);
        __CPROVER_assume(str_a);
        str_a->allocator = NULL;
        str_a->len = len_a;
        /* bytes are left nondet */
    }

    /* nondet allocation for str_b */
    if (__CPROVER_nondet_int()) {
        size_t len_b = __CPROVER_nondet_uint();
        __CPROVER_assume(len_b <= MAX_LEN);
        str_b = malloc(sizeof(struct aws_string) + len_b + 1);
        __CPROVER_assume(str_b);
        str_b->allocator = NULL;
        str_b->len = len_b;
        /* bytes are left nondet */
    }

    __CPROVER_assume(IMPLIES(str_a != NULL, aws_string_is_valid(str_a)));
    __CPROVER_assume(IMPLIES(str_b != NULL, aws_string_is_valid(str_b)));

    /* Snapshot original state for frame condition checks */
    struct aws_string *old_a = NULL;
    struct aws_string *old_b = NULL;

    if (str_a) {
        old_a = malloc(sizeof(struct aws_string) + str_a->len + 1);
        __CPROVER_assume(old_a);
        __CPROVER_assume(old_a != str_a);
        memcpy(old_a, str_a, sizeof(struct aws_string) + str_a->len + 1);
    }
    if (str_b) {
        old_b = malloc(sizeof(struct aws_string) + str_b->len + 1);
        __CPROVER_assume(old_b);
        __CPROVER_assume(old_b != str_b);
        memcpy(old_b, str_b, sizeof(struct aws_string) + str_b->len + 1);
    }

    bool result = aws_string_eq(str_a, str_b);

    /* Return‑value correctness */
    if (str_a == str_b) {
        assert(result);
    } else if (str_a == NULL || str_b == NULL) {
        assert(!result);
    } else {
        bool expected = false;
        if (str_a->len == str_b->len) {
            expected = (memcmp(str_a->bytes, str_b->bytes, str_a->len) == 0);
        }
        assert(result == expected);
    }

    /* Length invariants */
    if (str_a) {
        assert(str_a->len == old_a->len);
    }
    if (str_b) {
        assert(str_b->len == old_b->len);
    }

    /* Frame condition: bytes must remain unchanged */
    if (str_a) {
        assert(memcmp(str_a->bytes, old_a->bytes, str_a->len) == 0);
    }
    if (str_b) {
        assert(memcmp(str_b->bytes, old_b->bytes, str_b->len) == 0);
    }
}
