#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_harness() {
    /* Create two strings with nondet content */
    struct aws_string *a = nondet_allocate_string_bounded_length(4);
    __CPROVER_assume(aws_string_is_valid(a));

    struct aws_string *b = nondet_allocate_string_bounded_length(4);
    __CPROVER_assume(aws_string_is_valid(b));

    /* Save lengths before call */
    size_t a_len_before = a->len;
    size_t b_len_before = b->len;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: if result true, lengths must match */
    if (result == true) {
        assert(a->len == b->len);
    }

    /* Postcondition: if lengths differ, result must be false */
    if (a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: strings not modified */
    assert(a->len == a_len_before);
    assert(aws_string_is_valid(a));
    assert(b->len == b_len_before);
    assert(aws_string_is_valid(b));

    /* Result is boolean */
    assert(result == true || result == false);
}
