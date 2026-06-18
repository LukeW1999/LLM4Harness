#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STRING_LEN 64

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic strings a and b */
    struct aws_string *a;
    struct aws_string *b;

    /* nondet lengths, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_STRING_LEN);
    __CPROVER_assume(len_b <= MAX_STRING_LEN);

    /* allocate memory for the structs (flexible array member) */
    a = malloc(sizeof(struct aws_string) + (len_a > 0 ? len_a - 1 : 0));
    b = malloc(sizeof(struct aws_string) + (len_b > 0 ? len_b - 1 : 0));

    /* allocator can be NULL (static) or default; both are valid */
    a->allocator = aws_default_allocator();
    b->allocator = aws_default_allocator();

    a->len = len_a;
    b->len = len_b;

    /* fill bytes with nondet values */
    for (size_t i = 0; i < len_a; ++i) {
        a->bytes[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < len_b; ++i) {
        b->bytes[i] = nondet_uint8_t();
    }

    /* 2. Assume the strings are valid */
    __CPROVER_assume(aws_string_is_valid(a));
    __CPROVER_assume(aws_string_is_valid(b));

    /* 3. Save old state for immutability checks */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    uint8_t *old_a_bytes = NULL;
    uint8_t *old_b_bytes = NULL;
    if (len_a > 0) {
        old_a_bytes = malloc(len_a);
        memcpy(old_a_bytes, a->bytes, len_a);
    }
    if (len_b > 0) {
        old_b_bytes = malloc(len_b);
        memcpy(old_b_bytes, b->bytes, len_b);
    }

    /* 4. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 5. Postcondition: result matches specification */
    bool expected = false;
    if (a == b) {
        expected = true;
    } else if (a != NULL && b != NULL) {
        if (a->len == b->len) {
            bool bytes_eq = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_eq = false;
                    break;
                }
            }
            expected = bytes_eq;
        } else {
            expected = false;
        }
    } else {
        expected = false;
    }
    assert(result == expected);

    /* 6. Unchanged fields (immutability) */
    assert(a->allocator == old_a.allocator);
    assert(a->len == old_a.len);
    if (len_a > 0) {
        assert(memcmp(a->bytes, old_a_bytes, len_a) == 0);
    }

    assert(b->allocator == old_b.allocator);
    assert(b->len == old_b.len);
    if (len_b > 0) {
        assert(memcmp(b->bytes, old_b_bytes, len_b) == 0);
    }

    /* 7. Validity invariants must still hold */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));

    /* clean up */
    free(old_a_bytes);
    free(old_b_bytes);
    free(a);
    free(b);
}
