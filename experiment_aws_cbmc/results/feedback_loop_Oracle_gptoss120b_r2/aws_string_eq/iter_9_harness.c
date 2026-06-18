#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 8

void aws_string_eq_harness(void) {
    struct aws_string *str_a = 0;
    struct aws_string *str_b = 0;

    /* nondet allocation for str_a */
    if (__CPROVER_nondet_int()) {
        unsigned char buf_a[sizeof(struct aws_string) + MAX_LEN];
        str_a = (struct aws_string *)buf_a;
        str_a->allocator = aws_default_allocator();
        size_t len_a = __CPROVER_nondet_uint();
        __CPROVER_assume(len_a <= MAX_LEN);
        str_a->len = len_a;
        /* bytes left nondet */
    }

    /* nondet allocation for str_b */
    if (__CPROVER_nondet_int()) {
        unsigned char buf_b[sizeof(struct aws_string) + MAX_LEN];
        str_b = (struct aws_string *)buf_b;
        str_b->allocator = aws_default_allocator();
        size_t len_b = __CPROVER_nondet_uint();
        __CPROVER_assume(len_b <= MAX_LEN);
        str_b->len = len_b;
        /* bytes left nondet */
    }

    __CPROVER_assume(!(str_a != 0) || aws_string_is_valid(str_a));
    __CPROVER_assume(!(str_b != 0) || aws_string_is_valid(str_b));

    /* Snapshot original state for frame condition checks */
    unsigned char old_buf_a[sizeof(struct aws_string) + MAX_LEN];
    unsigned char old_buf_b[sizeof(struct aws_string) + MAX_LEN];
    struct aws_string *old_a = 0;
    struct aws_string *old_b = 0;

    if (str_a) {
        old_a = (struct aws_string *)old_buf_a;
        __CPROVER_assume(old_a != str_a);
        memcpy(old_a, str_a, sizeof(struct aws_string) + str_a->len);
    }
    if (str_b) {
        old_b = (struct aws_string *)old_buf_b;
        __CPROVER_assume(old_b != str_b);
        memcpy(old_b, str_b, sizeof(struct aws_string) + str_b->len);
    }

    bool result = aws_string_eq(str_a, str_b);

    /* Return‑value correctness */
    if (str_a == str_b) {
        assert(result);
    } else if (str_a == 0 || str_b == 0) {
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
