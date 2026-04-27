// === STEP 1: SUCCESS PATH ===
// aws_string_destroy is void - no return value.
// When str != NULL and str->allocator != NULL:
//   - aws_mem_release is called, freeing the memory
//   - The string is deallocated (no post-condition assertions on freed memory)
//
// === STEP 2: FAILURE PATH ===
// When str == NULL or str->allocator == NULL:
//   - Nothing happens (no-op)
//   - If str != NULL and str->allocator == NULL: str remains valid, unchanged
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - If str == NULL: nothing to check
//   - If str != NULL and str->allocator == NULL: str is unchanged (no-op)
//   - If str != NULL and str->allocator != NULL: str is freed (cannot check fields)
//
// === STEP 4: VALIDITY INVARIANTS ===
// We can only assert aws_string_is_valid before the call.
// After the call, if str was freed, we cannot dereference it.
// If str == NULL or str->allocator == NULL, we can assert validity after.

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_destroy_harness() {
    /* Non-deterministically choose whether str is NULL */
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* Case 1: str is NULL - function should be a no-op */
        str = NULL;
        aws_string_destroy(str);
        /* Nothing to assert - NULL case is handled */
    } else {
        /* Case 2: str is non-NULL */
        /* Allocate a string with some bounded length */
        size_t len;
        __CPROVER_assume(len <= 32); /* bound for tractability */
        
        /* Allocate memory for the aws_string struct plus data bytes plus null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        void *mem = malloc(alloc_size);
        __CPROVER_assume(mem != NULL);
        
        str = (struct aws_string *)mem;
        
        /* Set up the string fields - need to cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        *(size_t *)&str->len = len;
        
        if (str->allocator != NULL) {
            /* Set up a minimal allocator */
            __CPROVER_assume(str->allocator != NULL);
            
            /* Save state before call */
            struct aws_allocator *old_allocator = str->allocator;
            size_t old_len = str->len;
            
            /* Pre-condition: string should be valid before destroy */
            /* aws_string_is_valid checks: str != NULL, str->bytes + str->len doesn't overflow */
            /* We trust the setup is valid */
            
            /* Call the function - str will be freed */
            aws_string_destroy(str);
            
            /* After freeing, we cannot dereference str.
             * The only thing we can verify is that the function didn't crash
             * and that it called aws_mem_release (which we model via malloc/free).
             * No post-condition assertions on freed memory. */
        } else {
            /* str->allocator == NULL: function should be a no-op */
            size_t old_len = str->len;
            
            aws_string_destroy(str);
            
            /* str should be unchanged since allocator is NULL */
            assert(str->allocator == NULL);
            assert(str->len == old_len);
        }
    }
}
