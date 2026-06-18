#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    struct aws_string *a;
    struct aws_string *b;
    
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    
    size_t old_a_len = 0;
    size_t old_b_len = 0;
    struct aws_allocator *old_a_alloc = NULL;
    struct aws_allocator *old_b_alloc = NULL;
    
    if (!a_is_null) {
        a = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(a != NULL);
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_BUFFER_SIZE);
        a->allocator = aws_default_allocator();
        __CPROVER_assume(aws_string_is_valid(a));
        old_a_len = a->len;
        old_a_alloc = a->allocator;
    } else {
        a = NULL;
    }
    
    if (!b_is_null) {
        b = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(b != NULL);
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_BUFFER_SIZE);
        b->allocator = aws_default_allocator();
        __CPROVER_assume(aws_string_is_valid(b));
        old_b_len = b->len;
        old_b_alloc = b->allocator;
    } else {
        b = NULL;
    }
    
    bool result = aws_string_eq(a, b);
    
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (result) {
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
