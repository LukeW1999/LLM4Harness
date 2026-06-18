#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
struct aws_string *nondet_ptr(void);

/* harness */
void aws_string_eq_harness(void) {
    struct aws_string *a = nondet_ptr();
    struct aws_string *b = nondet_ptr();

    /* allocate and initialise a if non‑NULL */
    size_t len_a = 0;
    if (a != NULL) {
        len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        /* allocate enough space for flexible array member */
        a = malloc(sizeof(struct aws_string) + (len_a ? len_a - 1 : 0));
        __CPROVER_assume(a != NULL);
        a->allocator = NULL;               /* static string – allocator may be NULL */
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
    }

    /* allocate and initialise b if non‑NULL */
    size_t len_b = 0;
    if (b != NULL) {
        len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + (len_b ? len_b - 1 : 0));
        __CPROVER_assume(b != NULL);
        b->allocator = NULL;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(a != NULL, aws_string_is_valid(a)));
    __CPROVER_assume(IMPLIES(b != NULL, aws_string_is_valid(b)));

    /* snapshot of a */
    struct aws_allocator *a_alloc_snap = a ? a->allocator : NULL;
    size_t a_len_snap = a ? a->len : 0;
    uint8_t *a_bytes_snap = NULL;
    if (a) {
        a_bytes_snap = malloc(a_len_snap);
        __CPROVER_assume(a_bytes_snap != NULL);
        for (size_t i = 0; i < a_len_snap; ++i) {
            a_bytes_snap[i] = a->bytes[i];
        }
    }

    /* snapshot of b */
    struct aws_allocator *b_alloc_snap = b ? b->allocator : NULL;
    size_t b_len_snap = b ? b->len : 0;
    uint8_t *b_bytes_snap = NULL;
    if (b) {
        b_bytes_snap = malloc(b_len_snap);
        __CPROVER_assume(b_bytes_snap != NULL);
        for (size_t i = 0; i < b_len_snap; ++i) {
            b_bytes_snap[i] = b->bytes[i];
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq(a, b);

    /* ----- post‑condition checks ----- */

    /* 1. Return‑value correctness */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        bool bytes_equal = true;
        if (a_len_snap != b_len_snap) {
            bytes_equal = false;
        } else {
            for (size_t i = 0; i < a_len_snap; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_equal = false;
                    break;
                }
            }
        }
        assert(result == bytes_equal);
    }

    /* 2. No modification of inputs (frame condition) */
    if (a) {
        assert(a->allocator == a_alloc_snap);
        assert(a->len == a_len_snap);
        for (size_t i = 0; i < a_len_snap; ++i) {
            assert(a->bytes[i] == a_bytes_snap[i]);
        }
    }
    if (b) {
        assert(b->allocator == b_alloc_snap);
        assert(b->len == b_len_snap);
        for (size_t i = 0; i < b_len_snap; ++i) {
            assert(b->bytes[i] == b_bytes_snap[i]);
        }
    }

    return 0;
}
