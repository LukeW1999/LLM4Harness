#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *a;
    struct aws_string *b;

    /* a may be NULL or a valid string */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + (len_a == 0 ? 0 : len_a - 1));
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        /* bytes are nondeterministic, no need to initialise */
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + (len_b == 0 ? 0 : len_b - 1));
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = len_b;
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_a_struct;
    struct store_byte_from_buffer old_a_bytes;
    if (a) {
        old_a_struct = *a;                     /* copies allocator and len */
        save_byte_from_array(a->bytes, a->len, &old_a_bytes);
    }

    struct aws_string old_b_struct;
    struct store_byte_from_buffer old_b_bytes;
    if (b) {
        old_b_struct = *b;
        save_byte_from_array(b->bytes, b->len, &old_b_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Compute expected result according to specification */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }
    assert(result == expected);

    /* 5. Assert unchanged fields for a */
    if (a) {
        assert(a->allocator == old_a_struct.allocator);
        assert(a->len == old_a_struct.len);
        assert_byte_from_buffer_matches(a->bytes, &old_a_bytes);
    }

    /* 6. Assert unchanged fields for b */
    if (b) {
        assert(b->allocator == old_b_struct.allocator);
        assert(b->len == old_b_struct.len);
        assert_byte_from_buffer_matches(b->bytes, &old_b_bytes);
    }

    /* 7. Validity invariants must still hold */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
