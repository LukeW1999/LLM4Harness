/*  
 * Contract for aws_array_eq  
 * Preconditions:  
 *   - len_a is a size_t value.  
 *   - len_b is a size_t value.  
 *   - If len_a > 0, array_a points to a readable memory region of at least len_a bytes.  
 *   - If len_b > 0, array_b points to a readable memory region of at least len_b bytes.  
 *   - If len_a == 0, array_a may be NULL.  
 *   - If len_b == 0, array_b may be NULL.  
 * Postconditions (validity):  
 *   - The function returns a bool indicating whether the two arrays are equal.  
 *   - No memory is modified: the contents of array_a and array_b remain unchanged.  
 *   - No other memory locations are modified (frame condition).  
 * Postconditions (logical):  
 *   - If len_a != len_b, the result is false.  
 *   - If len_a == len_b == 0, the result is true.  
 *   - If len_a == len_b > 0, the result is true iff memcmp(array_a, array_b, len_a) == 0.  
 */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* nondet lengths */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* limit lengths to avoid excessive allocation */
    __CPROVER_assume(len_a <= 1024);
    __CPROVER_assume(len_b <= 1024);

    /* allocate buffers */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        /* make memory readable */
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

    /* keep copies for frame condition */
    uint8_t *copy_a = NULL;
    uint8_t *copy_b = NULL;

    if (len_a > 0) {
        copy_a = malloc(len_a);
        __CPROVER_assume(copy_a != NULL);
        memcpy(copy_a, array_a, len_a);
    }

    if (len_b > 0) {
        copy_b = malloc(len_b);
        __CPROVER_assume(copy_b != NULL);
        memcpy(copy_b, array_b, len_b);
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
        assert(memcmp(array_a, copy_a, len_a) == 0);
    }
    if (len_b > 0) {
        assert(memcmp(array_b, copy_b, len_b) == 0);
    }

    /* clean up */
    free(array_a);
    free(array_b);
    free(copy_a);
    free(copy_b);

    return 0;
}
