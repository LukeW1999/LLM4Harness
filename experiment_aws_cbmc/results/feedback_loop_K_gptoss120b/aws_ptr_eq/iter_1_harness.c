/*  
 * Contract for aws_ptr_eq  
 * -----------------------  
 * Preconditions:  
 *   - No specific preconditions; the pointers `a` and `b` may be any valid  
 *     pointer values, including NULL.  
 *   - The function does not dereference the pointers, so they need not point  
 *     to allocated memory.  
 *   - No allocator or other external resources are required.  
 *  
 * Postconditions (validity):  
 *   - The function returns a `bool` value.  
 *   - Return value is `true` iff the two input pointers compare equal (`a == b`).  
 *   - No side‑effects: the function does not modify any memory, nor does it  
 *     allocate or free memory.  
 *  
 * Postconditions (frame):  
 *   - The values of `a` and `b` remain unchanged after the call.  
 *   - No global or heap memory is modified.  
 */  

#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Declaration of the function under test */
bool aws_ptr_eq(const void *a, const void *b);

void aws_ptr_eq_harness(void) {
    /* Nondeterministic pointer values */
    uintptr_t a_val = (uintptr_t)malloc(1);   /* allocate a dummy object to obtain a valid address */
    uintptr_t b_val = (uintptr_t)malloc(1);
    __CPROVER_assume(a_val != 0);
    __CPROVER_assume(b_val != 0);
    /* Also allow NULL */
    if (__CPROVER_nondet_bool()) {
        a_val = 0;
    }
    if (__CPROVER_nondet_bool()) {
        b_val = 0;
    }

    const void *a = (const void *)a_val;
    const void *b = (const void *)b_val;

    /* Capture the original values for later frame checking */
    const void *a_orig = a;
    const void *b_orig = b;

    /* Call the function under verification */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (a == b));

    /* Frame conditions: input pointers must be unchanged */
    assert(a == a_orig);
    assert(b == b_orig);

    /* No heap modifications: free the dummy allocations */
    free((void *)a_val);
    free((void *)b_val);

    return 0;
}
