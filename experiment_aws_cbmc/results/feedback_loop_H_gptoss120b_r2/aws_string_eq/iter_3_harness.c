#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* a may be NULL or a valid string */
    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
        uint8_t a_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        a = (struct aws_string *)a_storage;
        a->allocator = aws_default_allocator();
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
        uint8_t b_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        b = (struct aws_string *)b_storage;
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
    struct store_byte_from_buffer a_bytes_store;
    struct store_byte_from_buffer b_bytes_store;

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

    /* 4. Post‑condition assertions */
    if (a == b) {
        /* same pointer (including both NULL) yields true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* one NULL and the other non‑NULL yields false */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct pointers */
        if (a->len != b->len) {
            assert(result == false);
        } else {
            bool bytes_equal = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_equal = false;
                    break;
                }
            }
            assert(result == bytes_equal);
        }
    }

    /* 5. Unchanged fields (function is pure) */
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
