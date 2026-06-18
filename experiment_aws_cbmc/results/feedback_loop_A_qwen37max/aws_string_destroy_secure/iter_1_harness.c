#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    bool has_allocator = false;
    
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        
        size_t alloc_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(alloc_size > len); /* prevent overflow */
        
        void *mem = malloc(alloc_size);
        str = (struct aws_string *)mem;
        
        if (str) {
            /* Bypass const fields for initialization */
            struct aws_string_mut {
                struct aws_allocator *allocator;
                size_t len;
                uint8_t bytes[1];
            };
            struct aws_string_mut *mut_str = (struct aws_string_mut *)str;
            mut_str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
            mut_str->len = len;
            has_allocator = (mut_str->allocator != NULL);
        }
    }

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy_secure(str);

    /* 
     * If the string was not freed (i.e. it had no allocator), 
     * assert that the data bytes were securely zeroed out.
     */
    if (str != NULL && !has_allocator) {
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == 0);
        }
    }
}
