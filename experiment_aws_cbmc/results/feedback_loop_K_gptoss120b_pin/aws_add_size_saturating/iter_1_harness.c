/*  
 * Contract for aws_add_size_saturating  
 *  
 * Preconditions:  
 *   - None (any size_t values for a and b are allowed).  
 *  
 * Postconditions (validity):  
 *   - The function returns a size_t value r such that:  
 *       * If a + b does not overflow, r == a + b.  
 *       * If a + b overflows, r == SIZE_MAX.  
 *   - No pointer arguments are used, so no null‑pointer checks are required.  
 *  
 * Postconditions (frame):  
 *   - The call must not modify any global or static state (no side effects).  
 *   - No memory is allocated or freed.  
 */  

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/config.h>
#include <aws/common/logging.h>
#include <aws/common/private/dlloads.h>
#include <aws/common/private/external_module_impl.h>
#include <aws/common/private/thread_shared.h>

#include "proof_helpers/make_common_data_structures.h"

void aws_add_size_saturating_harness(void) {
    /* nondet inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* Compute the expected result using unsigned overflow semantics */
    size_t sum = a + b;
    bool overflow = (sum < a); /* true iff overflow occurred */

    size_t expected = overflow ? SIZE_MAX : sum;

    /* Verify postconditions */
    assert(result == expected);

    return 0;
}
