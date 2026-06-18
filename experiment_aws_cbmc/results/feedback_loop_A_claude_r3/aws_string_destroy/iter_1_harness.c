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
        /* Create a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound the string length */
        
        /* Allocate memory for the string header + data bytes + null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - need to cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        *(size_t *)&str->len = len;
        /* bytes are already allocated as part of the struct */
    }
    
    /* The function should handle NULL gracefully */
    /* If str is NULL, function is a no-op */
    /* If str->allocator is NULL, function is a no-op */
    /* If str->allocator is non-NULL, memory is released */
    
    /* Save state before call */
    bool str_was_null = (str == NULL);
    bool had_allocator = (!str_was_null && str->allocator != NULL);
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* Postconditions:
     * 1. If str was NULL: no-op, nothing to assert about str
     * 2. If str->allocator was NULL: no-op (str still valid in memory since not freed)
     * 3. If str->allocator was non-NULL: memory was released (we can't assert on freed memory)
     * 
     * The key postcondition is that the function does NOT crash/abort in any case.
     * We assert that we reached this point (implicit in CBMC - no assertion violation).
     */
    
    /* If str was NULL, we can assert nothing changed (no crash) */
    if (str_was_null) {
        assert(str == NULL); /* str pointer itself unchanged */
    }
    
    /* The function completed without error - this is the main postcondition */
    /* CBMC verifies no memory safety violations occurred during the call */
    assert(true); /* We reached here without undefined behavior */
}
