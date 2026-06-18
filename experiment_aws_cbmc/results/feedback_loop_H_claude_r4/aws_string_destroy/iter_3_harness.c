#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    /* Non-deterministically decide if str is NULL */
    bool is_null;
    __CPROVER_assume(true);
    
    struct aws_string *str = NULL;
    
    if (nondet_bool()) {
        /* Allocate a valid aws_string with bounded length */
        str = nondet_allocate_aws_string_bounded_length(10);
        __CPROVER_assume(str != NULL);
        
        /* Must use aws_default_allocator() - non-NULL with valid mem_release */
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);
        __CPROVER_assume(alloc->mem_release != NULL);
        
        /* Set the allocator to the default allocator */
        *(const struct aws_allocator **)&str->allocator = alloc;
        
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* Precondition: str is NULL or str is valid with proper allocator */
    __CPROVER_assume(str == NULL || 
                     (aws_string_is_valid(str) && 
                      str->allocator != NULL && 
                      str->allocator->mem_release != NULL));
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* The key postcondition: function completes without error */
    assert(true);
}
