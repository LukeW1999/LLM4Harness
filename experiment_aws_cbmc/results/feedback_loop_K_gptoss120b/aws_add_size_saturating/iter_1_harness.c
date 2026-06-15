/*  
 * Contract for aws_add_size_saturating  
 * ------------------------------------  
 * Preconditions:  
 *   - None (a and b are nondeterministic size_t values).  
 *   - No pointer arguments, so no null‑pointer assumptions.  
 *   - No global state is required; the function must not depend on any initialized library.  
 *  
 * Postconditions (validity):  
 *   - The return value r is a size_t.  
 *   - If a + b does not overflow (i.e., a <= SIZE_MAX - b), then r == a + b.  
 *   - If a + b overflows, then r == SIZE_MAX.  
 *  
 * Postconditions (frame):  
 *   - The function must not modify any memory locations other than its return value.  
 *   - No global variables are read or written.  
 */  

#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic generators for size_t */
size_t nondet_size_t(void);
void *nondet_ptr(void);

void aws_add_size_saturating_harness(void) {
    /* Allocate nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* No additional assumptions needed; a and b can be any size_t value */

    /* Call the function under test */
    size_t r = aws_add_size_saturating(a, b);

    /* Compute the overflow condition manually */
    bool overflow = (a > SIZE_MAX - b);

    /* Postcondition checks */
    if (!overflow) {
        assert(r == a + b);
    } else {
        assert(r == SIZE_MAX);
    }

    /* Frame condition: the function must not have written to any memory location.
       Since the function has no pointer arguments and no global state, we simply
       assert that the only observable change is the return value. */
    /* No further assertions needed for memory frame. */

    return 0;
}

/* Definitions of nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    __CPROVER_assume(x == x); /* make x nondeterministic */
    return x;
}
void *nondet_ptr(void) {
    void *p;
    __CPROVER_assume(p == p);
    return p;
}
