#include <aws/common/hash_table.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ptr_eq_harness(void) {
    /* nondeterministic choice whether each pointer is NULL */
    bool a_is_null = __CPROVER_nondet_bool();
    bool b_is_null = __CPROVER_nondet_bool();

    const void *a = NULL;
    const void *b = NULL;

    /* allocate fixed-size buffers when not NULL */
    uint8_t *buf_a = NULL;
    uint8_t *buf_b = NULL;
    const size_t buf_size = 8U; /* small, bounded size */

    if (!a_is_null) {
        buf_a = malloc(buf_size);
        __CPROVER_assume(buf_a != NULL);
        /* fill with nondeterministic data */
        for (size_t i = 0U; i < buf_size; ++i) {
            buf_a[i] = __CPROVER_nondet_uint8_t();
        }
        a = buf_a;
    }

    if (!b_is_null) {
        buf_b = malloc(buf_size);
        __CPROVER_assume(buf_b != NULL);
        for (size_t i = 0U; i < buf_size; ++i) {
            buf_b[i] = __CPROVER_nondet_uint8_t();
        }
        b = buf_b;
    }

    /* snapshot the contents of the buffers */
    uint8_t snapshot_a[buf_size];
    uint8_t snapshot_b[buf_size];
    if (buf_a) {
        for (size_t i = 0U; i < buf_size; ++i) {
            snapshot_a[i] = buf_a[i];
        }
    }
    if (buf_b) {
        for (size_t i = 0U; i < buf_size; ++i) {
            snapshot_b[i] = buf_b[i];
        }
    }

    /* call the function under verification */
    bool result = aws_ptr_eq(a, b);

    /* postcondition 1: return value matches pointer equality */
    assert(result == (a == b));

    /* postcondition 2: buffers are unchanged (frame condition) */
    if (buf_a) {
        for (size_t i = 0U; i < buf_size; ++i) {
            assert(buf_a[i] == snapshot_a[i]);
        }
    }
    if (buf_b) {
        for (size_t i = 0U; i < buf_size; ++i) {
            assert(buf_b[i] == snapshot_b[i]);
        }
    }

    /* postcondition 3: the pointer arguments themselves are unchanged */
    assert(a_is_null ? a == NULL : a == (const void *)buf_a);
    assert(b_is_null ? b == NULL : b == (const void *)buf_b);

    /* clean up */
    free(buf_a);
    free(buf_b);
    return 0;
}
