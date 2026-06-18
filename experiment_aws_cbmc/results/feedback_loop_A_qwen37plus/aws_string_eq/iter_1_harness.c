#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (!a_is_null) {
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *temp_a = malloc(sizeof(*temp_a));
        __CPROVER_assume(temp_a != NULL);
        temp_a->allocator = NULL;
        temp_a->len = nondet_size_t();
        __CPROVER_assume(temp_a->len <= MAX_BUFFER_SIZE);
        a = (struct aws_string *)temp_a;
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *temp_b = malloc(sizeof(*temp_b));
        __CPROVER_assume(temp_b != NULL);
        temp_b->allocator = NULL;
        temp_b->len = nondet_size_t();
        __CPROVER_assume(temp_b->len <= MAX_BUFFER_SIZE);
        b = (struct aws_string *)temp_b;
        __CPROVER_assume(aws_string_is_valid(b));
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
