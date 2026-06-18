#include <aws/common/string.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (!a_is_null) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *tmp_a = malloc(sizeof(*tmp_a));
        __CPROVER_assume(tmp_a != NULL);
        tmp_a->allocator = aws_default_allocator();
        tmp_a->len = len_a;
        a = (struct aws_string *)tmp_a;
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *tmp_b = malloc(sizeof(*tmp_b));
        __CPROVER_assume(tmp_b != NULL);
        tmp_b->allocator = aws_default_allocator();
        tmp_b->len = len_b;
        b = (struct aws_string *)tmp_b;
        __CPROVER_assume(aws_string_is_valid(b));
    }

    bool result = aws_string_eq(a, b);

    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (a->len != b->len) {
            assert(result == false);
        } else {
            bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
            assert(result == expected);
        }
    }

    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
