#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    
    char c_str_arr[MAX_BUFFER_SIZE + 1];
    c_str_arr[c_str_len] = '\0';
    const char *c_str = c_str_arr;
    
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result != NULL) {
        /* Success path */
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_str_len);
        assert(aws_string_eq_c_str(result, c_str));
    } else {
        /* Failure path: returns NULL */
    }
}
