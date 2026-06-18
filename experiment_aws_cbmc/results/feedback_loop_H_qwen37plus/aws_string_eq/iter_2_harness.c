#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_string_eq_harness() {
    struct aws_string a;
    struct aws_string b;
    
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    
    struct aws_string *ptr_a = NULL;
    struct aws_string *ptr_b = NULL;
    
    size_t old_a_len = 0;
    size_t old_b_len = 0;
    struct aws_allocator *old_a_alloc = NULL;
    struct aws_allocator *old_b_alloc = NULL;
    
    if (!a_is_null) {
        ptr_a = &a;
        a.len = nondet_size_t();
        __CPROVER_assume(a.len <= MAX_BUFFER_SIZE);
        a.allocator = aws_default_allocator();
        __CPROVER_assume(aws_string_is_valid(ptr_a));
        old_a_len = a.len;
        old_a_alloc = a.allocator;
    }
    
    if (!b_is_null) {
        ptr_b = &b;
        b.len = nondet_size_t();
        __CPROVER_assume(b.len <= MAX_BUFFER_SIZE);
        b.allocator = aws_default_allocator();
        __CPROVER_assume(aws_string_is_valid(ptr_b));
        old_b_len = b.len;
        old_b_alloc = b.allocator;
    }
    
    bool result = aws_string_eq(ptr_a, ptr_b);
    
    if (ptr_a == ptr_b) {
        assert(result == true);
    } else if (ptr_a == NULL || ptr_b == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(ptr_a->len == ptr_b->len);
        }
    }
    
    if (ptr_a != NULL) {
        assert(ptr_a->len == old_a_len);
        assert(ptr_a->allocator == old_a_alloc);
        assert(aws_string_is_valid(ptr_a));
    }
    
    if (ptr_b != NULL) {
        assert(ptr_b->len == old_b_len);
        assert(ptr_b->allocator == old_b_alloc);
        assert(aws_string_is_valid(ptr_b));
    }
}
