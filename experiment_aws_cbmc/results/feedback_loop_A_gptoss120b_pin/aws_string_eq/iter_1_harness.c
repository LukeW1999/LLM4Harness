#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* nondeterministic pointers, possibly NULL */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    /* allocate a if not NULL */
    if (!a_is_null) {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_STRING_LEN);
        /* allocate enough space for flexible array member */
        a = malloc(sizeof(struct aws_string) + a_len);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = a_len;
        /* nondet fill bytes */
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* allocate b if not NULL */
    if (!b_is_null) {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + b_len);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* possibly make a and b point to the same object */
    bool same_ptr = nondet_bool();
    if (same_ptr && a != NULL && b != NULL) {
        free(b);
        b = a;
    }

    /* Save old state for immutability checks */
    struct aws_string old_a;
    uint8_t *old_a_bytes = NULL;
    if (a != NULL) {
        old_a = *a;
        old_a_bytes = malloc(old_a.len);
        __CPROVER_assume(old_a_bytes != NULL);
        for (size_t i = 0; i < old_a.len; ++i) {
            old_a_bytes[i] = a->bytes[i];
        }
    }

    struct aws_string old_b;
    uint8_t *old_b_bytes = NULL;
    if (b != NULL) {
        old_b = *b;
        old_b_bytes = malloc(old_b.len);
        __CPROVER_assume(old_b_bytes != NULL);
        for (size_t i = 0; i < old_b.len; ++i) {
            old_b_bytes[i] = b->bytes[i];
        }
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: result matches specification */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        if (a->len != b->len) {
            expected = false;
        } else {
            bool eq = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    eq = false;
                    break;
                }
            }
            expected = eq;
        }
    }
    assert(result == expected);

    /* Unchanged fields for a */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_bytes_match(a->bytes, old_a_bytes, a->len);
        assert(aws_string_is_valid(a));
    }

    /* Unchanged fields for b */
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_bytes_match(b->bytes, old_b_bytes, b->len);
        assert(aws_string_is_valid(b));
    }

    return 0;
}
