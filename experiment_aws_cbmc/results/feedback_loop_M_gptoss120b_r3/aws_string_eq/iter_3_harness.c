#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_harness(void) {
    /* nondeterministic choice of NULL or a valid string for a */
    bool a_is_null = nondet_bool();
    struct aws_string *a = NULL;
    size_t a_len = 0;
    struct aws_allocator *old_a_alloc = NULL;
    size_t old_a_len = 0;
    uint8_t a_bytes_copy[MAX_BUFFER_SIZE];

    if (!a_is_null) {
        a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);

        /* stack‑allocated storage for the aws_string object */
        uint8_t a_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE - 1];
        a = (struct aws_string *)a_storage;

        a->allocator = aws_default_allocator();
        a->len = a_len;
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));

        /* make a copy of the bytes for later immutability check */
        for (size_t i = 0; i < a_len; ++i) {
            a_bytes_copy[i] = a->bytes[i];
        }
        old_a_alloc = a->allocator;
        old_a_len   = a->len;
    }

    /* nondeterministic choice of NULL or a valid string for b */
    bool b_is_null = nondet_bool();
    struct aws_string *b = NULL;
    size_t b_len = 0;
    struct aws_allocator *old_b_alloc = NULL;
    size_t old_b_len = 0;
    uint8_t b_bytes_copy[MAX_BUFFER_SIZE];

    if (!b_is_null) {
        b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);

        uint8_t b_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE - 1];
        b = (struct aws_string *)b_storage;

        b->allocator = aws_default_allocator();
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));

        for (size_t i = 0; i < b_len; ++i) {
            b_bytes_copy[i] = b->bytes[i];
        }
        old_b_alloc = b->allocator;
        old_b_len   = b->len;
    }

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: return value semantics */
    if (a == b) {
        assert(result);
    } else if (a == NULL || b == NULL) {
        assert(!result);
    } else {
        bool eq = true;
        if (a->len != b->len) {
            eq = false;
        } else {
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    eq = false;
                }
            }
        }
        assert(result == eq);
    }

    /* Unchanged fields for a (if non‑NULL) */
    if (a != NULL) {
        assert(a->allocator == old_a_alloc);
        assert(a->len == old_a_len);
        assert_bytes_match(a->bytes, a_bytes_copy, a_len);
        assert(aws_string_is_valid(a));
    }

    /* Unchanged fields for b (if non‑NULL) */
    if (b != NULL) {
        assert(b->allocator == old_b_alloc);
        assert(b->len == old_b_len);
        assert_bytes_match(b->bytes, b_bytes_copy, b_len);
        assert(aws_string_is_valid(b));
    }
}
