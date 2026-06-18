#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
        a = malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        
        struct aws_string_mut {
            struct aws_allocator *allocator;
            size_t len;
        };
        struct aws_string_mut *a_mut = (struct aws_string_mut *)a;
        a_mut->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        a_mut->len = len_a;
        ((uint8_t *)a->bytes)[len_a] = 0;
        
        __CPROVER_assume(aws_string_is_valid(a));
    }

    struct aws_string *b = NULL;
    if (nondet_bool()) {
        if (nondet_bool() && a != NULL) {
            b = a;
        } else {
            size_t len_b = nondet_size_t();
            __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
            b = malloc(sizeof(struct aws_string) + len_b);
            __CPROVER_assume(b != NULL);
            
            struct aws_string_mut {
                struct aws_allocator *allocator;
                size_t len;
            };
            struct aws_string_mut *b_mut = (struct aws_string_mut *)b;
            b_mut->allocator = nondet_bool() ? aws_default_allocator() : NULL;
            b_mut->len = len_b;
            ((uint8_t *)b->bytes)[len_b] = 0;
            
            __CPROVER_assume(aws_string_is_valid(b));
        }
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

    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
