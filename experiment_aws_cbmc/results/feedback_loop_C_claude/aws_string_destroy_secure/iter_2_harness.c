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
        /* Non-NULL case: allocate a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for aws_string header + len bytes */
        size_t total_size = sizeof(struct aws_string) + len;
        /* Avoid overflow */
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size >= len);
        
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - need to write through const */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        } *writable = (void *)str;
        
        /* Non-deterministically choose allocator (NULL = static, non-NULL = dynamic) */
        if (nondet_bool()) {
            writable->allocator = NULL;
        } else {
            writable->allocator = aws_default_allocator();
        }
        writable->len = len;
        
        /* Fill bytes with non-deterministic data */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        
        /* Verify the string is valid before the call */
        assert(aws_string_is_valid(str));
        
        /* Save state before call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        
        /* If allocator is NULL (static string), we can check bytes after */
        bool has_allocator = (str->allocator != NULL);
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /* Post-call assertions */
        if (!has_allocator) {
            /* Memory was NOT freed, we can still access str */
            /* len should be unchanged (const field) */
            assert(str->len == old_len);
            /* allocator should still be NULL */
            assert(str->allocator == old_allocator);
            /* bytes should be zeroed */
            if (len > 0) {
                for (size_t i = 0; i < len; i++) {
                    assert(str->bytes[i] == 0);
                }
            }
        }
        /* If has_allocator, memory was freed - cannot dereference str */
    }
}
