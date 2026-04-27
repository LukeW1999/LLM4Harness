#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *a;
    struct aws_string *b;
    size_t max_length = 100; // Arbitrary bound for the length of the strings

    a = malloc(sizeof(*a) + max_length);
    b = malloc(sizeof(*b) + max_length);

    __CPROVER_assume(a != NULL && b != NULL);
    __CPROVER_assume(a->allocator == NULL); // Assuming allocator is not used in this function
    __CPROVER_assume(b->allocator == NULL); // Assuming allocator is not used in this function
    __CPROVER_assume(a->len <= max_length);
    __CPROVER_assume(b->len <= max_length);

    for (size_t i = 0; i < a->len; i++) {
        a->bytes[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < b->len; i++) {
        b->bytes[i] = nondet_uint8_t();
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (a == b) {
        assert(result == true);
    } else {
        if (result == true) {
            assert(a->len == b->len);
            assert_bytes_match(a->bytes, b->bytes, a->len);
        } else {
            // No specific changes to fields on failure
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a->allocator == old_a.allocator);
    assert(a->len == old_a.len);
    assert(b->allocator == old_b.allocator);
    assert(b->len == old_b.len);

    /* 6. Assert validity invariants always holds */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));

    free(a);
    free(b);
}
