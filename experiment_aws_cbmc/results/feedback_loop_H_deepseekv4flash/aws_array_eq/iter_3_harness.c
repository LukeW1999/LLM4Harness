#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    size_t len_a, len_b;
    uint8_t *array_a = NULL, *array_b = NULL;
    bool equal_contents;

    /* Nondeterministic sizes, bounded to small values */
    len_a = nondet_size_t();
    len_b = nondet_size_t();
    __CPROVER_assume(len_a <= 5 && len_b <= 5);

    /* Allocate arrays if length > 0 */
    if (len_a > 0) {
        array_a = (uint8_t *)malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        /* Fill with nondeterministic bytes */
        for (size_t i = 0; i < len_a; i++) {
            array_a[i] = nondet_uint8_t();
        }
    }
    if (len_b > 0) {
        array_b = (uint8_t *)malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        /* We want to cover both equal and unequal cases:
           If len_a == len_b > 0, nondeterministically decide if arrays are equal */
        if (len_a == len_b && len_a > 0) {
            equal_contents = nondet_bool();
            if (equal_contents) {
                /* Make array_b identical to array_a */
                for (size_t i = 0; i < len_a; i++) {
                    array_b[i] = array_a[i];
                }
            } else {
                /* Make array_b differ from array_a in at least one position */
                for (size_t i = 0; i < len_a; i++) {
                    array_b[i] = nondet_uint8_t();
                }
                /* Ensure at least one byte differs (if possible) */
                if (len_a > 0) {
                    __CPROVER_assume(array_b[0] != array_a[0]);
                }
            }
        } else {
            /* Lengths differ or one is zero; fill b with nondet */
            for (size_t i = 0; i < len_b; i++) {
                array_b[i] = nondet_uint8_t();
            }
        }
    }

    /* Call the function */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postcondition checks */
    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        assert(result == true);
    } else {
        /* len_a == len_b > 0 */
        if (result == true) {
            /* All bytes must match */
            for (size_t i = 0; i < len_a; i++) {
                assert(array_a[i] == array_b[i]);
            }
        } else {
            /* At least one byte must differ */
            bool found_diff = false;
            for (size_t i = 0; i < len_a; i++) {
                if (array_a[i] != array_b[i]) {
                    found_diff = true;
                    break;
                }
            }
            assert(found_diff);
        }
    }

    /* Cleanup */
    free(array_a);
    free(array_b);
}
