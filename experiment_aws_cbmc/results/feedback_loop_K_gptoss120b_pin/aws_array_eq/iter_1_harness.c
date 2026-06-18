/*  
 * Contract for aws_array_eq  
 * Preconditions:  
 *   - If len_a > 0, array_a points to a readable memory region of at least len_a bytes.  
 *   - If len_b > 0, array_b points to a readable memory region of at least len_b bytes.  
 *   - If len_a == 0, array_a may be NULL.  
 *   - If len_b == 0, array_b may be NULL.  
 * Postconditions (validity):  
 *   - The function returns a bool.  
 *   - No memory is modified by the function (the contents of the input arrays remain unchanged).  
 * Postconditions (length):  
 *   - The return value is true iff len_a == len_b and the first len_a bytes of array_a and array_b are equal.  
 *   - If len_a != len_b, the function returns false.  
 *   - If len_a == len_b == 0, the function returns true.  
 * Postconditions (frame):  
 *   - No side‑effects on any global state or allocator.  
 */

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_eq_harness(void) {
    /* nondet lengths */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* constrain lengths to a reasonable bound to avoid overflow in CBMC */
    __CPROVER_assume(len_a <= 1024);
    __CPROVER_assume(len_b <= 1024);

    /* allocate buffers according to lengths */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        /* make the memory readable and nondet */
        for (size_t i = 0; i < len_a; ++i) {
            array_a[i] = nondet_uint8_t();
        }
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            array_b[i] = nondet_uint8_t();
        }
    }

    /* preserve copies for frame condition */
    uint8_t *array_a_copy = NULL;
    uint8_t *array_b_copy = NULL;
    if (len_a > 0) {
        array_a_copy = malloc(len_a);
        __CPROVER_assume(array_a_copy != NULL);
        memcpy(array_a_copy, array_a, len_a);
    }
    if (len_b > 0) {
        array_b_copy = malloc(len_b);
        __CPROVER_assume(array_b_copy != NULL);
        memcpy(array_b_copy, array_b, len_b);
    }

    /* call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* compute expected result */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }

    /* postcondition: result matches specification */
    assert(result == expected);

    /* frame condition: inputs unchanged */
    if (len_a > 0) {
        assert(memcmp(array_a, array_a_copy, len_a) == 0);
    }
    if (len_b > 0) {
        assert(memcmp(array_b, array_b_copy, len_b) == 0);
    }

    /* clean up */
    free(array_a);
    free(array_b);
    free(array_a_copy);
    free(array_b_copy);

    return 0;
}
