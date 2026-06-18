#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    struct aws_allocator *allocator = aws_default_allocator();
    
    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        
        str->len = len;
        str->allocator = allocator;
    }
    
    aws_string_destroy_secure(str);
}
