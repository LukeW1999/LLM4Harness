#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_destroy_harness() {
    /* Create a valid aws_string using the helper */
    struct aws_string *str = nondet_allocate_string_bounded_length(32);
    
    /* Either str is NULL or it is a valid aws_string */
    if (str == NULL) {
        /* aws_string_destroy should handle NULL gracefully */
        aws_string_destroy(str);
        return;
    }
    
    /* Ensure the string is valid before calling destroy */
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Call the function */
    aws_string_destroy(str);
}
