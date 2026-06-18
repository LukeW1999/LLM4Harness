#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_array_eq_harness(void) {
    /* Nondeterministic inputs */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* Bound lengths to keep the model tractable */
    __CPROVER_assume(len_a <= 1024);
    __CPROVER_assume(len_b <= 1024);

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(array_a, len_a));
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(array_b, len_b));
    }

    /* Precondition assumptions */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    /* Snapshot of inputs for post‑condition checks */
    size_t saved_len_a = len_a;
    size_t saved_len_b = len_b;
    uint8_t *saved_a = NULL;
    uint8_t *saved_b = NULL;

    if (len_a > 0) {
        saved_a = malloc(len_a);
        __CPROVER_assume(saved_a != NULL);
        memcpy(saved_a, array_a, len_a);
    }

    if (len_b > 0) {
        saved_b = malloc(len_b);
        __CPROVER_assume(saved_b != NULL);
        memcpy(saved_b, array_b, len_b);
    }

    /* Call the function under verification */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* Lengths must be unchanged */
    assert(len_a == saved_len_a);
    assert(len_b == saved_len_b);

    /* Memory pointed to by the arrays must be unchanged */
    if (len_a > 0) {
        for (size_t i = 0; i < len_a; ++i) {
            assert(array_a[i] == saved_a[i]);
        }
    }
    if (len_b > 0) {
        for (size_t i = 0; i < len_b; ++i) {
            assert(array_b[i] == saved_b[i]);
        }
    }

    /* Result must reflect equality of the two arrays */
    if (len_a != len_b) {
        assert(result == false);
    } else {
        bool eq = true;
        for (size_t i = 0; i < len_a; ++i) {
            if (array_a[i] != array_b[i]) {
                eq = false;
                break;
            }
        }
        assert(result == eq);
    }
}
