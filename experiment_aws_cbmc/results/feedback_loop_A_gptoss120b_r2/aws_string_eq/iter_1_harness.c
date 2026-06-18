#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* a may be NULL or a valid string */
    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        /* nondeterministically fill the bytes */
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + len_b);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_a = {0};
    struct aws_string old_b = {0};
    struct store_byte_from_buffer a_bytes_store = {0};
    struct store_byte_from_buffer b_bytes_store = {0};

    if (a) {
        old_a = *a;
        save_byte_from_array(a->bytes, a->len, &a_bytes_store);
    }
    if (b) {
        old_b = *b;
        save_byte_from_array(b->bytes, b->len, &b_bytes_store);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Postcondition assertions */
    if (a == b) {
        /* both pointers equal (including both NULL) */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* one is NULL, the other is not */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct pointers */
        bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (a) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_store);
    }
    if (b) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_store);
    }

    /* 6. Validity invariants must still hold */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
