/*  
 * Contract for aws_mul_size_saturating  
 * Preconditions: none (a and b are arbitrary size_t values)  
 * Postconditions (validity): the function returns a size_t value r such that  
 *   - if a * b does not overflow size_t, then r == a * b  
 *   - otherwise r == SIZE_MAX  
 * Postconditions (frame): the function does not modify any memory locations (pure function)  
 */

#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* No additional assumptions needed; a and b can be any size_t values */

    size_t result = aws_mul_size_saturating(a, b);

    /* Compute the mathematically correct product using a wider type */
    unsigned __int128 prod = (unsigned __int128)a * (unsigned __int128)b;
    size_t expected = (prod > (unsigned __int128)SIZE_MAX) ? SIZE_MAX : (size_t)prod;

    /* Verify the postcondition */
    assert(result == expected);

    return 0;
}
