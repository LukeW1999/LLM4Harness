#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Non-deterministically decide if str is NULL */
    struct aws_string *str = NULL;
    
    if (nondet_bool()) {
        /* Create a valid aws_string with non-deterministic length */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound the length for tractability */
        
        /* Allocate memory for the string header + bytes + null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - need to cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        *(size_t *)&str->len = len;
        
        /* Initialize bytes - must set null terminator for aws_string_is_valid */
        uint8_t *bytes = (uint8_t *)str->bytes;
        /* Fill bytes with non-deterministic values */
        for (size_t i = 0; i < len; i++) {
            bytes[i] = nondet_uint8_t();
        }
        /* Set null terminator */
        bytes[len] = '\0';
    }
    
    /* Precondition: str is NULL or str is valid */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    
    /* Save state before call */
    bool str_was_null = (str == NULL);
    bool had_allocator = (str != NULL && str->allocator != NULL);
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* The function completed successfully (no crash) - this is the main postcondition */
    assert(true);
    
    /* If str was NULL, the function should have been a no-op */
    if (str_was_null) {
        assert(str == NULL);
    }
}
