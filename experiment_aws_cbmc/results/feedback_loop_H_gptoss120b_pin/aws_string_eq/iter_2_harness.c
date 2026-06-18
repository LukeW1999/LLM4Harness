#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* a may be NULL or a valid string */
    if (!nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t buf_a[MAX_STRING_LEN];
        a = aws_string_new_from_array(aws_default_allocator(), buf_a, len_a);
        __CPROVER_assume(a != NULL);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (!nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t buf_b[MAX_STRING_LEN];
        b = aws_string_new_from_array(aws_default_allocator(), buf_b, len_b);
        __CPROVER_assume(b != NULL);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old state for a */
    struct aws_string old_a_struct;
    struct store_byte_from_buffer old_a_bytes;
    if (a) {
        old_a_struct = *a;
        save_byte_from_array(a->bytes, a->len, &old_a_bytes);
    }

    /* Save old state for b */
    struct aws_string old_b_struct;
    struct store_byte_from_buffer old_b_bytes;
    if (b) {
        old_b_struct = *b;
        save_byte_from_array(b->bytes, b->len, &old_b_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Compute expected result */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }
    assert(result == expected);

    /* Assert unchanged fields for a */
    if (a) {
        assert(a->allocator == old_a_struct.allocator);
        assert(a->len == old_a_struct.len);
        assert_byte_from_buffer_matches(a->bytes, &old_a_bytes);
    }

    /* Assert unchanged fields for b */
    if (b) {
        assert(b->allocator == old_b_struct.allocator);
        assert(b->len == old_b_struct.len);
        assert_byte_from_buffer_matches(b->bytes, &old_b_bytes);
    }

    /* Validity invariants must still hold */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
