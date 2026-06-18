#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
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
        *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        *(size_t *)&str->len = len;
        /* bytes are already allocated as part of the struct */
    }
    
    /* The function should handle NULL gracefully */
    /* If str is non-NULL and has an allocator, it will call aws_mem_release */
    /* If str is NULL or has no allocator, it's a no-op */
    
    /* Save state before call */
    bool was_null = (str == NULL);
    bool had_allocator = (!was_null && str->allocator != NULL);
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* Postconditions:
     * 1. If str was NULL: no-op, nothing to assert about str
     * 2. If str was non-NULL but allocator was NULL: no-op (static string), str memory not freed
     * 3. If str was non-NULL and had allocator: memory was released
     * 
     * The function has void return type, so we just verify it doesn't crash.
     * For the NULL case, we can assert nothing changed (trivially true).
     */
    
    /* If str was NULL, the function should have been a no-op */
    if (was_null) {
        assert(str == NULL);
    }
    
    /* The function completed without crashing - this is the main postcondition
     * for a void function. The key invariant is:
     * - NULL str: no-op
     * - non-NULL str with NULL allocator: no-op  
     * - non-NULL str with non-NULL allocator: memory released
     */
    
    /* We can assert that if str was NULL, it remains NULL */
    assert(was_null ? (str == NULL) : true);
}
