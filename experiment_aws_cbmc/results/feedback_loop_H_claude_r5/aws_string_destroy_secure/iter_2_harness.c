#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether str is NULL or not */
    struct aws_string *str = ensure_string_is_allocated_nondet_length();
    
    /* str may be NULL from ensure_string_is_allocated_nondet_length */
    /* The precondition for aws_string_destroy_secure is: !str || aws_string_is_valid(str) */
    
    if (str != NULL) {
        /* Ensure the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Save the length before the call */
        size_t old_len = str->len;
        
        /* Save the allocator before the call */
        struct aws_allocator *old_allocator = (struct aws_allocator *)str->allocator;
        
        /* Precondition must hold */
        assert(aws_string_is_valid(str));
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /*
         * After the call:
         * - If allocator was NULL: bytes are zeroed, memory NOT freed
         * - If allocator was non-NULL: bytes are zeroed, memory IS freed
         *
         * We can only safely check postconditions when allocator was NULL
         * (memory not freed).
         */
        if (old_allocator == NULL) {
            /*
             * With NULL allocator, memory was not freed.
             * The bytes should have been zeroed (aws_secure_zero was called on len bytes).
             * We can still access str safely.
             */
            if (old_len > 0) {
                const uint8_t *bytes = aws_string_bytes(str);
                assert(bytes[0] == 0);
            }
            
            /* The len field itself should be unchanged */
            assert(str->len == old_len);
            
            /* The allocator should still be NULL */
            assert(str->allocator == NULL);
            
            /* Free the memory ourselves since allocator was NULL */
            free(str);
        }
        /* If allocator was non-NULL, memory was freed - don't access str */
    } else {
        /* Test NULL case: function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* If we reach here without crash, the NULL case is handled correctly */
        assert(true);
    }
}
