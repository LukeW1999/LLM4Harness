#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_string *str;
    
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    
    if (str != NULL) {
        __CPROVER_assume(str->allocator == NULL || str->allocator == aws_default_allocator());
    }
    
    bool is_no_op = (str == NULL || str->allocator == NULL);
    
    aws_string_destroy(str);
    
    if (is_no_op) {
        assert(str == NULL || aws_string_is_valid(str));
    }
}
