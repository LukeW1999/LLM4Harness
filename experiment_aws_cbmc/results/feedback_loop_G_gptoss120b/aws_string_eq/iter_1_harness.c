#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include "aws/common/string.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_SIZE 64

void aws_string_eq_harness(void) {
    struct aws_string *a;
    struct aws_string *b;

    /* nondeterministically decide if a is NULL or a valid string */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_SIZE);
        a = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE - 1);
        __CPROVER_assume(a != NULL);
        a->len = len_a;
        a->allocator = NULL; /* allocator is not used in equality check */
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* nondeterministically decide if b is NULL or a valid string */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_SIZE);
        b = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE - 1);
        __CPROVER_assume(b != NULL);
        b->len = len_b;
        b->allocator = NULL;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old state before the call */
    struct aws_string old_a;
    struct aws_string old_b;
    if (a != NULL) {
        old_a = *a;
    }
    if (b != NULL) {
        old_b = *b;
    }

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions: no fields of a or b are modified */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_bytes_match(a->bytes, old_a.bytes, a->len);
    }
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_bytes_match(b->bytes, old_b.bytes, b->len);
    }

    /* Validity invariants must still hold */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* The return value is a boolean, no further assertions needed */
}
