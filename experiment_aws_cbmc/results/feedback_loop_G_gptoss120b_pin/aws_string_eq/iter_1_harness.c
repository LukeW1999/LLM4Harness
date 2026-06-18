#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *a;
    struct aws_string *b;

    /* nondet decide if a is NULL */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= 256);
        a = (struct aws_string *)malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* nondet decide if b is NULL */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= 256);
        b = (struct aws_string *)malloc(sizeof(struct aws_string) + len_b);
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
    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;

    if (a != NULL) {
        old_a = *a;
        save_byte_from_array(a->bytes, a->len, &a_storage);
    }
    if (b != NULL) {
        old_b = *b;
        save_byte_from_array(b->bytes, b->len, &b_storage);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Post‑condition assertions */
    if (a == b) {
        /* same pointer (including both NULL) yields true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* one NULL and the other non‑NULL yields false */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct */
        if (result) {
            /* equality implies same length and identical bytes */
            assert(a->len == b->len);
            assert_bytes_match(a->bytes, b->bytes, a->len);
        } else {
            /* inequality implies either length differs or some byte differs */
            /* No additional concrete assertion required beyond the above */
        }
    }

    /* 5. Unchanged fields (immutability) */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_byte_from_buffer_matches(a->bytes, &a_storage);
    }
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_byte_from_buffer_matches(b->bytes, &b_storage);
    }

    /* 6. Validity invariants must still hold */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));
}
