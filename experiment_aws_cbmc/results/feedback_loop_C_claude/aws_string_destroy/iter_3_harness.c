#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_destroy_harness() {
    /* Case: str->allocator == NULL - function should be a no-op */
    size_t len;
    __CPROVER_assume(len <= 32);
    size_t alloc_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)malloc(alloc_size);
    __CPROVER_assume(str != NULL);
    
    /* Set allocator to NULL */
    *(struct aws_allocator **)&str->allocator = NULL;
    *(size_t *)&str->len = len;
    
    /* Fill bytes field with something valid */
    for (size_t i = 0; i <= len; i++) {
        ((char *)str->bytes)[i] = 'a';
    }
    
    /* Verify precondition: allocator is NULL */
    assert(str->allocator == NULL);
    assert(str->len == len);
    
    size_t old_len = str->len;
    
    /* Call the function - with NULL allocator, should be a no-op */
    aws_string_destroy(str);
    
    /* Post-condition: str should be unchanged since allocator is NULL */
    assert(str->allocator == NULL);
    assert(str->len == old_len);
}
