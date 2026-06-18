#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    size_t a_len = nondet_size_t();
    size_t b_len = nondet_size_t();
    __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);

    if (nondet_bool()) {
        a = malloc(sizeof(struct aws_string) + a_len);
        __CPROVER_assume(a != NULL);
        struct aws_string *a_mut = (struct aws_string *)a;
        a_mut->len = a_len;
        a_mut->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    }

    if (nondet_bool()) {
        b = malloc(sizeof(struct aws_string) + b_len);
        __CPROVER_assume(b != NULL);
        struct aws_string *b_mut = (struct aws_string *)b;
        b_mut->len = b_len;
        b_mut->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    }

    bool result = aws_string_eq(a, b);

    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (a->len != b->len) {
            assert(result == false);
        }
    }
}
