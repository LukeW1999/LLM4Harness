#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 64

void aws_string_eq_harness(void) {
    /* 1. Declare pointers for the two strings */
    struct aws_string *a;
    struct aws_string *b;

    /* 2. Non‑deterministically decide whether each pointer is NULL */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        a->allocator = NULL;               /* static string – allocator not used */
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            ((uint8_t *)a->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + len_b);
        __CPROVER_assume(b != NULL);
        b->allocator = NULL;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            ((uint8_t *)b->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 3. Save old immutable state for later comparison */
    struct aws_allocator *old_a_allocator = NULL;
    size_t old_a_len = 0;
    struct store_byte_from_buffer old_a_bytes = {0};

    if (a != NULL) {
        old_a_allocator = a->allocator;
        old_a_len = a->len;
        save_byte_from_array(a->bytes, a->len, &old_a_bytes);
    }

    struct aws_allocator *old_b_allocator = NULL;
    size_t old_b_len = 0;
    struct store_byte_from_buffer old_b_bytes = {0};

    if (b != NULL) {
        old_b_allocator = b->allocator;
        old_b_len = b->len;
        save_byte_from_array(b->bytes, b->len, &old_b_bytes);
    }

    /* 4. Call the function under test */
    bool result = aws_string_eq(a, b);

    /* 5. Post‑condition: functional correctness */
    if (a == b) {
        /* same pointer (including both NULL) → true */
        assert(result);
    } else if (a == NULL || b == NULL) {
        /* one is NULL, the other not → false */
        assert(!result);
    } else {
        /* both non‑NULL and distinct → result must equal array equality */
        bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == expected);
    }

    /* 6. Unchanged fields for a */
    if (a != NULL) {
        assert(a->allocator == old_a_allocator);
        assert(a->len == old_a_len);
        assert_bytes_match(a->bytes, old_a_bytes.buffer, old_a_len);
    }

    /* 7. Unchanged fields for b */
    if (b != NULL) {
        assert(b->allocator == old_b_allocator);
        assert(b->len == old_b_len);
        assert_bytes_match(b->bytes, old_b_bytes.buffer, old_b_len);
    }

    /* 8. Validity invariants must still hold */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
