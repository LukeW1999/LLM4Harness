#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* Non-deterministically decide if str is NULL */
    struct aws_string *str = NULL;
    
    if (nondet_bool()) {
        /* Use the helper to create a valid aws_string */
        str = nondet_allocate_aws_string_bounded_length(10);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Optionally set allocator to NULL or default */
        if (nondet_bool()) {
            *(struct aws_allocator **)&str->allocator = NULL;
        } else {
            *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        }
    }
    
    /* Precondition: str is NULL or str is valid */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* Verify we reached here without undefined behavior */
    assert(true);
}
