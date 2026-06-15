#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic pointers (may be NULL) */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* a may be NULL or a valid string */
    bool a_is_null = nondet_bool();
    __CPROVER_assume(a_is_null == true || a_is_null == false);
    if (!a_is_null) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
        size_t alloc_a = sizeof(struct aws_string) + (len_a > 0 ? len_a - 1 : 0);
        a = (struct aws_string *)malloc(alloc_a);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    bool b_is_null = nondet_bool();
    __CPROVER_assume(b_is_null == true || b_is_null == false);
    if (!b_is_null) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
        size_t alloc_b = sizeof(struct aws_string) + (len_b > 0 ? len_b - 1 : 0);
        b = (struct aws_string *)malloc(alloc_b);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_a;
    struct aws_string old_b;
    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;

    if (a != NULL) {
        old_a = *a;
        if (a->len > 0) {
            save_byte_from_array(a->bytes, a->len, &old_a_byte);
        }
    }
    if (b != NULL) {
        old_b = *b;
        if (b->len > 0) {
            save_byte_from_array(b->bytes, b->len, &old_b_byte);
        }
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Post‑condition: result matches specification */
    if (a == b) {
        /* same pointer (including both NULL) */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* one is NULL, the other is not */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct */
        bool eq = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == eq);
    }

    /* 5. Unchanged fields for a */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        if (a->len > 0) {
            assert_byte_from_buffer_matches(a->bytes, &old_a_byte);
        }
    }

    /* 6. Unchanged fields for b */
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        if (b->len > 0) {
            assert_byte_from_buffer_matches(b->bytes, &old_b_byte);
        }
    }

    /* 7. Validity invariants after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
