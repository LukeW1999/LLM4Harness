#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    if (nondet_bool()) {
        a = ensure_string_is_valid();
        __CPROVER_assume(aws_string_is_valid(a));
    }

    struct aws_string *b = NULL;
    if (nondet_bool()) {
        b = ensure_string_is_valid();
        __CPROVER_assume(aws_string_is_valid(b));
    }

    if (a != NULL) {
        __CPROVER_assume(__CPROVER_r_ok(a->bytes, a->len));
    }
    if (b != NULL) {
        __CPROVER_assume(__CPROVER_r_ok(b->bytes, b->len));
    }

    size_t old_a_len = a ? a->len : 0;
    struct aws_allocator *old_a_alloc = a ? a->allocator : NULL;
    
    size_t old_b_len = b ? b->len : 0;
    struct aws_allocator *old_b_alloc = b ? b->allocator : NULL;

    bool result = aws_string_eq(a, b);

    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (result == true) {
            assert(a->len == b->len);
        }
    }

    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(a->allocator == old_a_alloc);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(b->allocator == old_b_alloc);
        assert(aws_string_is_valid(b));
    }
}
