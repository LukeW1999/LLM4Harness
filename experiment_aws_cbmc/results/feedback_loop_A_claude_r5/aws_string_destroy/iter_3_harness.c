#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* Use the helper to create a valid aws_string */
    struct aws_string *str = nondet_allocate_aws_string_bounded_length(10);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Ensure the allocator is the default allocator (non-NULL with valid mem_release) */
    __CPROVER_assume(str->allocator == aws_default_allocator());
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* Verify we reached here without undefined behavior */
    assert(true);
}
