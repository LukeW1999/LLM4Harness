#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        
        if (is_static) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool was_null = (str == NULL);
    bool was_static = (str != NULL && str->allocator == NULL);
    
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    aws_string_destroy(str);

    if (was_null) {
        assert(str == NULL);
    } else if (was_static) {
        assert(str != NULL);
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(aws_string_is_valid(str));
    } else {
        /* Dynamically allocated string is freed.
         * We cannot assert anything about `str` as it is deallocated. */
    }
}
