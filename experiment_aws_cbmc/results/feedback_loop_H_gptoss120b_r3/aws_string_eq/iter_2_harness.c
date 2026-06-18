#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* nondeterministic allocation for string a */
    struct aws_string *a = NULL;
    if (nondet_bool()) {
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } a_obj;
        a = &a_obj.s;
        a->allocator = aws_default_allocator();
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_STRING_LEN);
        for (size_t i = 0; i < a->len; ++i) {
            a_obj.bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* nondeterministic allocation for string b */
    struct aws_string *b = NULL;
    if (nondet_bool()) {
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } b_obj;
        b = &b_obj.s;
        b->allocator = aws_default_allocator();
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_STRING_LEN);
        for (size_t i = 0; i < b->len; ++i) {
            b_obj.bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* possibly make a and b point to the same object */
    if (a && b && nondet_bool()) {
        b = a;
    }

    /* Save old state for immutability checks */
    struct aws_string old_a;
    struct store_byte_from_buffer a_storage;
    if (a) {
        old_a = *a;
        save_byte_from_array(a->bytes, a->len, &a_storage);
    }

    struct aws_string old_b;
    struct store_byte_from_buffer b_storage;
    if (b) {
        old_b = *b;
        save_byte_from_array(b->bytes, b->len, &b_storage);
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: result true iff strings are equal */
    if (result) {
        if (a != b) {
            assert(a != NULL);
            assert(b != NULL);
            assert(a->len == b->len);
            assert_bytes_match(a->bytes, b->bytes, a->len);
        }
    } else {
        assert(a != b);
        if (a && b) {
            /* If both non‑NULL, then either lengths differ or bytes differ */
            if (a->len == b->len) {
                /* Ensure there exists at least one differing byte */
                bool diff = false;
                for (size_t i = 0; i < a->len; ++i) {
                    if (a->bytes[i] != b->bytes[i]) {
                        diff = true;
                        break;
                    }
                }
                assert(diff);
            }
        }
    }

    /* Unchanged fields for a */
    if (a) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_byte_from_buffer_matches(a->bytes, &a_storage);
    }

    /* Unchanged fields for b */
    if (b) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_byte_from_buffer_matches(b->bytes, &b_storage);
    }

    /* Validity invariants */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
