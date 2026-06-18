#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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

    void *array_a = NULL;
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    void *array_b = NULL;
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        assert(result == true);
    } else {
        bool expected = true;
        for (size_t i = 0; i < len_a; i++) {
            if (((const uint8_t *)array_a)[i] != ((const uint8_t *)array_b)[i]) {
                expected = false;
                break;
            }
        }
        assert(result == expected);
    }

    if (array_a != NULL) {
        free(array_a);
    }
    if (array_b != NULL) {
        free(array_b);
    }
}
