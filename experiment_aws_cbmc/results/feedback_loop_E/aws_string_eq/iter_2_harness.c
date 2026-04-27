#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <string.h>

void aws_string_eq_harness() {
    /* parameters */
    struct aws_string *a;
    struct aws_string *b;

    /* assumptions */
    a = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    b = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(a != NULL && b != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(a->bytes, MAX_STRING_LEN));
    __CPROVER_assume(AWS_MEM_IS_READABLE(b->bytes, MAX_STRING_LEN));
    a->len = nondet_size_t();
    b->len = nondet_size_t();
    __CPROVER_assume(a->len <= MAX_STRING_LEN);
    __CPROVER_assume(b->len <= MAX_STRING_LEN);
    a->allocator = NULL; /* Assuming allocator is NULL for simplicity */
    b->allocator = NULL; /* Assuming allocator is NULL for simplicity */

    /* initialize bytes with arbitrary values */
    for (size_t i = 0; i < a->len; i++) {
        a->bytes[i] = nondet_char();
    }
    for (size_t i = 0; i < b->len; i++) {
        b->bytes[i] = nondet_char();
    }

    /* save old state */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    /* operation under verification */
    bool result = aws_string_eq(a, b);

    /* assertions */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        assert(result == (a->len == b->len && memcmp(a->bytes, b->bytes, a->len) == 0));
    }

    /* unchanged fields */
    assert(a->len == old_a.len);
    assert(b->len == old_b.len);
    assert(a->allocator == old_a.allocator);
    assert(b->allocator == old_b.allocator);
    assert(AWS_MEM_IS_READABLE(a->bytes, a->len));
    assert(AWS_MEM_IS_READABLE(b->bytes, b->len));

    /* validity invariants */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));
}
