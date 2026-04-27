#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Harness for aws_array_eq.
 *
 * Function behavior:
 * - Returns false if len_a != len_b
 * - Returns true if len_a == len_b == 0
 * - Returns true if len_a == len_b and memcmp(array_a, array_b, len_a) == 0
 * - Returns false if len_a == len_b and memcmp(array_a, array_b, len_a) != 0
 *
 * Preconditions:
 * - array_a must be readable for len_a bytes (or len_a == 0)
 * - array_b must be readable for len_b bytes (or len_b == 0)
 */
void aws_array_eq_harness() {
    /* Bound the sizes to keep the state space manageable */
    size_t len_a;
    size_t len_b;
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postcondition 1: If lengths differ, result must be false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* Postcondition 2: If lengths are equal */
    if (len_a == len_b) {
        if (len_a == 0) {
            /* Both empty arrays are equal */
            assert(result == true);
        } else {
            /* Result matches memcmp */
            if (result) {
                /* If true, memcmp must be 0 */
                assert(memcmp(array_a, array_b, len_a) == 0);
            } else {
                /* If false, memcmp must be non-zero */
                assert(memcmp(array_a, array_b, len_a) != 0);
            }
        }
    }

    /* Postcondition 3: result is a valid bool */
    assert(result == true || result == false);
}
