#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_harness(void) {
    /* 1. Non‑deterministic lengths, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. Allocate buffers according to lengths */
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

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Post‑condition assertions */
    if (result) {
        /* lengths must be equal */
        assert(len_a == len_b);
        /* if length > 0, all bytes must match */
        if (len_a > 0) {
            for (size_t i = 0; i < len_a; ++i) {
                assert(array_a[i] == array_b[i]);
            }
        }
    } else {
        /* either lengths differ, or at least one byte differs */
        if (len_a == len_b) {
            if (len_a == 0) {
                /* impossible: zero‑length equal arrays would return true */
                assert(0);
            } else {
                bool diff_found = false;
                for (size_t i = 0; i < len_a; ++i) {
                    if (array_a[i] != array_b[i]) {
                        diff_found = true;
                        break;
                    }
                }
                assert(diff_found);
            }
        } else {
            assert(len_a != len_b);
        }
    }

    /* 5. Clean up */
    free(array_a);
    free(array_b);
}
