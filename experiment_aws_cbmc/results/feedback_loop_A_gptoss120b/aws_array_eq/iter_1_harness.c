#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_eq_harness(void) {
    /* 1. Non‑deterministic lengths bounded by the test harness limits */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. Allocate buffers (NULL when length is zero) */
    void *array_a = NULL;
    void *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        for (size_t i = 0; i < len_a; ++i) {
            ((uint8_t *)array_a)[i] = nondet_uint8_t();
        }
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            ((uint8_t *)array_b)[i] = nondet_uint8_t();
        }
    }

    /* 3. Save old memory state for immutability checks */
    struct store_byte_from_buffer old_a = {0};
    struct store_byte_from_buffer old_b = {0};

    if (len_a > 0) {
        save_byte_from_array((const uint8_t *)array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array((const uint8_t *)array_b, len_b, &old_b);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Post‑condition: return value matches specification */
    if (result) {
        /* Success: lengths equal and contents equal (or both zero) */
        assert(len_a == len_b);
        if (len_a == 0) {
            /* both empty – nothing more to check */
        } else {
            assert(memcmp(array_a, array_b, len_a) == 0);
        }
    } else {
        /* Failure: either lengths differ or contents differ when non‑empty */
        assert(!(len_a == len_b && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0)));
    }

    /* 6. Immutability: input buffers must not be modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_a, &old_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_b, &old_b);
    }

    /* 7. Clean up (optional for CBMC) */
    free(array_a);
    free(array_b);
}
