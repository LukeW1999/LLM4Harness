#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        for (size_t i = 0; i < len_a; i++) {
            array_a[i] = nondet_uint8_t();
        }
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        for (size_t i = 0; i < len_b; i++) {
            array_b[i] = nondet_uint8_t();
        }
    }

    /* Save old contents for immutability check */
    uint8_t *old_a = NULL;
    uint8_t *old_b = NULL;
    if (len_a > 0) {
        old_a = malloc(len_a);
        __CPROVER_assume(old_a != NULL);
        memcpy(old_a, array_a, len_a);
    }
    if (len_b > 0) {
        old_b = malloc(len_b);
        __CPROVER_assume(old_b != NULL);
        memcpy(old_b, array_b, len_b);
    }

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Compute expected result according to specification */
    bool expected = false;
    if (len_a == len_b) {
        if (len_a == 0) {
            expected = true;
        } else {
            expected = true;
            for (size_t i = 0; i < len_a; i++) {
                if (array_a[i] != array_b[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    /* Assert arrays unchanged (const correctness) */
    if (len_a > 0) {
        assert(memcmp(array_a, old_a, len_a) == 0);
        free(old_a);
    }
    if (len_b > 0) {
        assert(memcmp(array_b, old_b, len_b) == 0);
        free(old_b);
    }

    /* Free allocated memory */
    if (len_a > 0) free(array_a);
    if (len_b > 0) free(array_b);
}
