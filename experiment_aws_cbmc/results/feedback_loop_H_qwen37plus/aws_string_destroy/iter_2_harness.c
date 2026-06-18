#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    bool is_static = nondet_bool();
    
    if (!is_null) {
        str = malloc(sizeof(struct aws_string) + 1024);
        __CPROVER_assume(str != NULL);
        
        if (is_static) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= 1024);
        
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool was_null = (str == NULL);
    bool was_static = (str != NULL && str->allocator == NULL);
    
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    aws_string_destroy(str);

    if (was_null) {
        assert(str == NULL);
    } else if (was_static) {
        assert(str != NULL);
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(aws_string_is_valid(str));
    } else {
        /* Dynamically allocated string is freed.
         * We cannot assert anything about `str` as it is deallocated. */
    }
}
