#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether str is NULL or not */
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* NULL case */
        str = NULL;
        /* Call with NULL - should be a no-op */
        aws_string_destroy_secure(str);
        /* No assertions needed - just verify no crash */
    } else {
        /* Non-NULL case: use ensure_string_is_allocated to get a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        str = ensure_string_is_allocated(len);
        __CPROVER_assume(str != NULL);
        
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Save state before call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        bool has_allocator = (str->allocator != NULL);
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /* Post-call assertions */
        if (!has_allocator) {
            /* Memory was NOT freed, we can still access str */
            /* bytes should be zeroed */
            if (old_len > 0) {
                for (size_t i = 0; i < old_len; i++) {
                    assert(str->bytes[i] == 0);
                }
            }
        }
        /* If has_allocator, memory was freed - cannot dereference str */
    }
}
