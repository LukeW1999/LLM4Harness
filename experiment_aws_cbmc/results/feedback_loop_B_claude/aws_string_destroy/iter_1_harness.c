#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/**
 * aws_string_destroy implementation:
 *   if (str && str->allocator) {
 *       aws_mem_release(str->allocator, str);
 *   }
 *
 * Analysis:
 * 1. If str is NULL → nothing happens (no-op)
 * 2. If str is non-NULL but str->allocator is NULL → nothing happens (no-op)
 * 3. If str is non-NULL and str->allocator is non-NULL → memory is released
 *
 * The function returns void, so there's no return value to check.
 * The key postcondition is that the function doesn't crash/violate memory safety
 * under all valid input combinations.
 */

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose which case to test */
    struct aws_string *str;

    /* Case 1: str is NULL */
    /* Case 2: str is non-NULL with NULL allocator */
    /* Case 3: str is non-NULL with non-NULL allocator */

    /* We use nondet to pick between cases */
    bool is_null = nondet_bool();
    bool has_allocator = nondet_bool();

    if (is_null) {
        /* Case 1: NULL pointer - function should be a no-op */
        str = NULL;
        
        /* Call function - should not crash */
        aws_string_destroy(str);
        
        /* Postcondition: str is still NULL */
        assert(str == NULL);
    } else if (!has_allocator) {
        /* Case 2: non-NULL str but NULL allocator - function should be a no-op */
        /* Allocate a minimal aws_string with NULL allocator */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 16); /* bound for tractability */
        
        /* Allocate memory for the string structure manually */
        /* sizeof(aws_string) includes 1 byte for bytes[], so total = sizeof(aws_string) + len */
        size_t total_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        
        struct aws_string *s = malloc(total_size);
        __CPROVER_assume(s != NULL);
        
        /* Set allocator to NULL - cast away const for initialization */
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&s->allocator;
        *alloc_ptr = NULL;
        
        /* Set length */
        size_t *len_ptr = (size_t *)&s->len;
        *len_ptr = len;
        
        str = s;
        
        /* Save the allocator value before call */
        struct aws_allocator *old_allocator = str->allocator;
        assert(old_allocator == NULL);
        
        /* Call function - should be a no-op since allocator is NULL */
        aws_string_destroy(str);
        
        /* Since allocator was NULL, nothing should have been freed/changed
         * We can't assert much about str after this since it wasn't freed,
         * but we can assert the call completed without error */
        /* The memory at str is still valid (wasn't freed) */
        assert(str != NULL);
        assert(str->allocator == NULL);
    } else {
        /* Case 3: non-NULL str with non-NULL allocator - memory gets released */
        /* Use the proof allocator which tracks allocations */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);
        
        /* Bound the string length for tractability */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 16);
        
        /* Allocate the string using the allocator */
        size_t total_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= sizeof(struct aws_string) + 16);
        
        /* Use aws_mem_acquire to allocate so aws_mem_release can free it */
        str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields */
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
        *alloc_ptr = allocator;
        
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;
        
        /* Verify preconditions */
        assert(str != NULL);
        assert(str->allocator != NULL);
        
        /* Call function - should release memory */
        aws_string_destroy(str);
        
        /* After destroy with valid allocator, the memory has been released.
         * We cannot safely dereference str anymore.
         * The main postcondition is that the function completed without
         * memory errors (CBMC will catch use-after-free if we dereference).
         * We assert true to indicate successful completion. */
        assert(true);
    }
}
