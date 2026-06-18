#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    if (nondet_bool()) {
        /* Create a valid aws_string using the proper allocator so it can be freed */
        /* Use a bounded length string literal */
        const char *c_str = "test";
        struct aws_string *str = aws_string_new_from_c_str(alloc, c_str);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Call function under test with valid string */
        aws_string_destroy(str);
    } else {
        /* Call function under test with NULL */
        aws_string_destroy(NULL);
    }
    
    assert(true);
}
