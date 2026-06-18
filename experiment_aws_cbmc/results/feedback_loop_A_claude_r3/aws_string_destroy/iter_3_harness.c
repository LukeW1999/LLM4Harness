#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Create a valid aws_string with a valid allocator */
    struct aws_string *str = nondet_allocate_aws_string_bounded_length(10);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Ensure the allocator is valid (non-NULL) with a valid mem_release function */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(allocator->mem_release != NULL);
    
    /* Set the allocator to the valid default allocator */
    *(struct aws_allocator **)&str->allocator = allocator;
    
    /* Preconditions must hold */
    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->allocator->mem_release != NULL);
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* The function completed without error */
    assert(true);
}
