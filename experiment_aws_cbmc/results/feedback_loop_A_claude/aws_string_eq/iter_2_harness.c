#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct aws_string *make_test_string(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    *(size_t *)&str->len = len;
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    int scenario;
    __CPROVER_assume(scenario >= 0 && scenario <= 4);

    if (scenario == 0) {
        /* Both NULL */
        a = NULL;
        b = NULL;
    } else if (scenario == 1) {
        /* a NULL, b valid */
        a = NULL;
        b = make_test_string(4);
        __CPROVER_assume(aws_string_is_valid(b));
    } else if (scenario == 2) {
        /* a valid, b NULL */
        a = make_test_string(4);
        __CPROVER_assume(aws_string_is_valid(a));
        b = NULL;
    } else if (scenario == 3) {
        /* Same pointer */
        a = make_test_string(4);
        __CPROVER_assume(aws_string_is_valid(a));
        b = a;
    } else {
        /* Both valid, independent */
        a = make_test_string(4);
        __CPROVER_assume(aws_string_is_valid(a));
        b = make_test_string(4);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save lengths before call */
    size_t a_len_before = (a != NULL) ? a->len : 0;
    size_t b_len_before = (b != NULL) ? b->len : 0;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: if same pointer and non-NULL, must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition: if either is NULL, result must be false */
    if (a == NULL || b == NULL) {
        assert(result == false);
    }

    /* Postcondition: if result true and both non-NULL, lengths must match */
    if (result == true && a != NULL && b != NULL) {
        assert(a->len == b->len);
    }

    /* Postcondition: if both non-NULL and lengths differ, result must be false */
    if (a != NULL && b != NULL && a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: strings not modified */
    if (a != NULL) {
        assert(a->len == a_len_before);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == b_len_before);
        assert(aws_string_is_valid(b));
    }

    /* Result is boolean */
    assert(result == true || result == false);
}
