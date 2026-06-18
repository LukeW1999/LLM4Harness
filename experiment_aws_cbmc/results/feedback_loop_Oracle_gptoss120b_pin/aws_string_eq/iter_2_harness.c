#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* possibly allocate a */
    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t a_buf[MAX_STRING_LEN];
        for (size_t i = 0; i < len_a; ++i) {
            a_buf[i] = nondet_uint8_t();
        }
        a = aws_string_new_from_array(allocator, a_buf, len_a);
        __CPROVER_assume(a != NULL);
    }

    /* possibly allocate b */
    if (nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t b_buf[MAX_STRING_LEN];
        for (size_t i = 0; i < len_b; ++i) {
            b_buf[i] = nondet_uint8_t();
        }
        b = aws_string_new_from_array(allocator, b_buf, len_b);
        __CPROVER_assume(b != NULL);
    }

    /* structural validity assumptions */
    __CPROVER_assume(!a || aws_string_is_valid(a));
    __CPROVER_assume(!b || aws_string_is_valid(b));

    /* snapshot of a */
    struct aws_allocator *a_alloc_snap = a ? a->allocator : NULL;
    size_t a_len_snap = a ? a->len : 0;
    uint8_t a_bytes_snap[MAX_STRING_LEN];
    if (a) {
        for (size_t i = 0; i < a_len_snap; ++i) {
            a_bytes_snap[i] = a->bytes[i];
        }
    }

    /* snapshot of b */
    struct aws_allocator *b_alloc_snap = b ? b->allocator : NULL;
    size_t b_len_snap = b ? b->len : 0;
    uint8_t b_bytes_snap[MAX_STRING_LEN];
    if (b) {
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
