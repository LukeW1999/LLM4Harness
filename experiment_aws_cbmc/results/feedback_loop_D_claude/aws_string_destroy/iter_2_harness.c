#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* Case 1: NULL pointer - function should do nothing */
        str = NULL;
        /* Call should not crash */
        aws_string_destroy(str);
        /* No assertions needed - just verify no crash */
    } else if (nondet_bool()) {
        /* Case 2: Non-NULL string with NULL allocator - function should do nothing */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 16); /* bound the size */
        
        /* We need to create a string with a NULL allocator */
        /* Use a stack-allocated structure to simulate this */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[17]; /* len + 1 for null terminator */
        } str_storage;
        
        /* Force allocator to NULL */
        str_storage.allocator = NULL;
        str_storage.len = len;
        
        str = (struct aws_string *)&str_storage;
        
        /* Save state before call */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;
        
        /* Call should not crash and should do nothing */
        aws_string_destroy(str);
        
        /* Verify nothing changed (allocator was NULL, so no free happened) */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
    } else {
        /* Case 3: Non-NULL string with valid allocator - function should free */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);
        
        /* Create a bounded string length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 16);
        
        /* Allocate the string using the allocator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= sizeof(struct aws_string) + 17);
        
        str = aws_mem_acquire(allocator, total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields */
        *(struct aws_allocator **)&str->allocator = allocator;
        *(size_t *)&str->len = len;
        
        /* Call should free the memory without crashing */
        aws_string_destroy(str);
        
        /* After freeing, we cannot dereference str - just verify no crash occurred */
    }
}
