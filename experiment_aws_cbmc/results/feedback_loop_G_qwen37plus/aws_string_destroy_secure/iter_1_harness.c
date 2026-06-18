#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    
    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        
        /* The function is not safe to run on a string created with AWS_STATIC_STRING_FROM_LITERAL,
         * which has allocator == NULL. Therefore, we assume it has a valid allocator. */
        str->allocator = aws_default_allocator();
        __CPROVER_assume(str->allocator != NULL);
        
        str->len = len;
    }

    aws_string_destroy_secure(str);

    if (is_null) {
        assert(str == NULL);
    } else {
        assert(str != NULL);
        /* Note: The memory pointed to by str is deallocated by the function.
         * CBMC will verify memory safety (e.g., valid pointer and size for aws_secure_zero 
         * and aws_mem_release). We cannot assert properties about the freed memory contents. */
    }
}
