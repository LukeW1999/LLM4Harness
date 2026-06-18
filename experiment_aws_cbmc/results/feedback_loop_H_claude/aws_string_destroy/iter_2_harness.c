#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* Non-deterministically decide if str is NULL */
    struct aws_string *str = NULL;
    
    if (nondet_bool()) {
        /* Create a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound the string length */
        
        /* Allocate memory for the string header + data bytes + null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - need to cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        
        /* Must set the null terminator at bytes[len] for aws_string_is_valid */
        uint8_t *bytes_ptr = (uint8_t *)str->bytes;
        bytes_ptr[len] = '\0';
    }
    
    /* Precondition: str must be NULL or a valid aws_string */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    
    /* Save state before call */
    bool was_null = (str == NULL);
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* If str was NULL, the function should have been a no-op */
    if (was_null) {
        assert(str == NULL);
    }
    
    /* We can assert that if str was NULL, it remains NULL */
    assert(was_null ? (str == NULL) : true);
}
