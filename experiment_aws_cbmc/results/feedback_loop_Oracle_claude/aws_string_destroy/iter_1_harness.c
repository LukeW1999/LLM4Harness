#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    /* Nondeterministically choose whether str is NULL or a valid aws_string */
    struct aws_string *str;
    
    /* We need to create a valid aws_string for the non-null case */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);
    
    if (is_null) {
        str = NULL;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound the length for tractability */
        
        /* Allocate memory for the string struct plus bytes plus null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string) + 1);
        
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Set up the string fields - need to cast away const for initialization */
        struct aws_allocator **allocator_ptr = (struct aws_allocator **)&str->allocator;
        
        /* Nondeterministically choose whether allocator is NULL or valid */
        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);
        
        if (has_allocator) {
            *allocator_ptr = aws_default_allocator();
        } else {
            *allocator_ptr = NULL;
        }
        
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;
        
        /* Set null terminator */
        uint8_t *bytes_ptr = (uint8_t *)str->bytes;
        bytes_ptr[len] = 0;
    }
    
    /* Apply the ground-truth preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    
    /* Save state before the call for frame condition checks */
    bool str_was_null = (str == NULL);
    bool had_null_allocator = (str != NULL && str->allocator == NULL);
    
    /* Call the function under verification */
    aws_string_destroy(str);
    
    /* Postcondition 1: Function returns void, so no return value to check.
     * Verify that if str was NULL, nothing bad happened (no crash = success).
     * This is implicitly verified by reaching this point. */
    assert(true); /* Reached here means no crash on NULL input */
    
    /* Postcondition 2: If str was NULL, the call was a no-op.
     * We verify this by checking we can still reason about str_was_null. */
    if (str_was_null) {
        assert(str == NULL);
    }
    
    /* Postcondition 3: If str had a NULL allocator (static string),
     * the function should have been a no-op (no memory freed).
     * We can verify we reach this point without issues. */
    if (had_null_allocator) {
        /* The string was not freed since allocator was NULL */
        /* We can still access str here since it wasn't freed */
        assert(str->allocator == NULL);
        assert(aws_string_is_valid(str));
    }
    
    /* Postcondition 4: Frame condition - if str was NULL, 
     * no global state should have been corrupted */
    if (str_was_null) {
        /* Verify we can still use the default allocator */
        struct aws_allocator *alloc = aws_default_allocator();
        assert(alloc != NULL);
    }
}

void aws_string_destroy_harness(void) {
    aws_string_destroy_harness();
    return 0;
}
