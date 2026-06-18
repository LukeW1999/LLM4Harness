#include <stdbool.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 64

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic strings a and b */
    uint8_t a_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
    uint8_t b_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *a = (struct aws_string *)a_buf;
    struct aws_string *b = (struct aws_string *)b_buf;

    /* nondet lengths, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_STRING_LEN);
    __CPROVER_assume(len_b <= MAX_STRING_LEN);

    /* allocator can be NULL (static) or default; both are valid */
    struct aws_allocator *allocator = aws_default_allocator();
    a->allocator = allocator;
    b->allocator = allocator;

    a->len = len_a;
    b->len = len_b;

    /* fill bytes with nondet values */
    for (size_t i = 0; i < len_a; ++i) {
        a->bytes[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < len_b; ++i) {
        b->bytes[i] = nondet_uint8_t();
    }

    /* 2. Assume the strings are valid */
    __CPROVER_assume(aws_string_is_valid(a));
    __CPROVER_assume(aws_string_is_valid(b));

    /* 3. Save old state for immutability checks */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    uint8_t old_a_bytes[MAX_STRING_LEN];
    uint8_t old_b_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len_a; ++i) {
        old_a_bytes[i] = a->bytes[i];
    }
    for (size_t i = 0; i < len_b; ++i) {
        old_b_bytes[i] = b->bytes[i];
    }

    /* 4. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 5. Postcondition: result matches specification */
    bool expected = false;
    if (a == b) {
        expected = true;
    } else if (a != NULL && b != NULL) {
        if (a->len == b->len) {
            bool bytes_eq = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_eq = false;
                    break;
                }
            }
            expected = bytes_eq;
        }
    }
    assert(result == expected);

    /* 6. Unchanged fields (immutability) */
    assert(a->allocator == old_a.allocator);
    assert(a->len == old_a.len);
    for (size_t i = 0; i < len_a; ++i) {
        assert(a->bytes[i] == old_a_bytes[i]);
    }

    assert(b->allocator == old_b.allocator);
    assert(b->len == old_b.len);
    for (size_t i = 0; i < len_b; ++i) {
        assert(b->bytes[i] == old_b_bytes[i]);
    }

    /* 7. Validity invariants must still hold */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));
}
