#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 8

void aws_string_eq_harness(void) {
    struct aws_string *str_a = 0;
    struct aws_string *str_b = 0;

    /* nondet allocation for str_a */
    if (__CPROVER_nondet_int()) {
        union {
            struct aws_string s;
            unsigned char raw[sizeof(struct aws_string) + MAX_LEN];
        } u_a;
        str_a = &u_a.s;
        str_a->allocator = aws_default_allocator();
        size_t len_a = __CPROVER_nondet_uint();
        __CPROVER_assume(len_a <= MAX_LEN);
        str_a->len = len_a;
        /* bytes left nondet */
    }

    /* nondet allocation for str_b */
    if (__CPROVER_nondet_int()) {
        union {
            struct aws_string s;
            unsigned char raw[sizeof(struct aws_string) + MAX_LEN];
        } u_b;
        str_b = &u_b.s;
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
        for (size_t i = 0; i < sizeof(struct aws_string) + str_a->len; ++i) {
            old_buf_a[i] = ((unsigned char *)str_a)[i];
        }
    }
    if (str_b) {
        old_b = (struct aws_string *)old_buf_b;
        __CPROVER_assume(old_b != str_b);
        for (size_t i = 0; i < sizeof(struct aws_string) + str_b->len; ++i) {
            old_buf_b[i] = ((unsigned char *)str_b)[i];
        }
    }

    _Bool result = aws_string_eq(str_a, str_b);

    /* Return‑value correctness */
    if (str_a == str_b) {
        __CPROVER_assert(result, "result must be true when pointers are equal");
    } else if (str_a == 0 || str_b == 0) {
        __CPROVER_assert(!result, "result must be false when either pointer is null");
    } else {
        _Bool expected = 0;
        if (str_a->len == str_b->len) {
            expected = 1;
            for (size_t i = 0; i < str_a->len; ++i) {
                if (str_a->bytes[i] != str_b->bytes[i]) {
                    expected = 0;
                    break;
                }
            }
        }
        __CPROVER_assert(result == expected, "result must match expected equality");
    }

    /* Length invariants */
    if (str_a) {
        __CPROVER_assert(str_a->len == old_a->len, "len of str_a unchanged");
    }
    if (str_b) {
        __CPROVER_assert(str_b->len == old_b->len, "len of str_b unchanged");
    }

    /* Frame condition: bytes must remain unchanged */
    if (str_a) {
        for (size_t i = 0; i < str_a->len; ++i) {
            __CPROVER_assert(str_a->bytes[i] == old_a->bytes[i],
                             "bytes of str_a unchanged");
        }
    }
    if (str_b) {
        for (size_t i = 0; i < str_b->len; ++i) {
            __CPROVER_assert(str_b->bytes[i] == old_b->bytes[i],
                             "bytes of str_b unchanged");
        }
    }
}
