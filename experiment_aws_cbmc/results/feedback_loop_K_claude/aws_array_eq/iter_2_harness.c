#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ARRAY_LEN 8

void aws_array_eq_harness(void) {
    /* Symbolic lengths */
    size_t len_a;
    size_t len_b;

    /* Bound lengths for CBMC tractability */
    __CPROVER_assume(len_a <= MAX_ARRAY_LEN);
    __CPROVER_assume(len_b <= MAX_ARRAY_LEN);

    /* Allocate arrays based on lengths */
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

    /* Postcondition 1: if lengths differ, result must be false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* Postcondition 2: if both lengths are zero, result must be true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* Postcondition 3: if lengths are equal and non-zero,
       result is true iff contents are identical */
    if (len_a == len_b && len_a > 0) {
        bool contents_equal = true;
        for (size_t i = 0; i < len_a; i++) {
            if (array_a[i] != array_b[i]) {
                contents_equal = false;
                break;
            }
        }
        assert(result == contents_equal);
    }

    /* Postcondition 4: result is a valid bool */
    assert(result == true || result == false);
}
