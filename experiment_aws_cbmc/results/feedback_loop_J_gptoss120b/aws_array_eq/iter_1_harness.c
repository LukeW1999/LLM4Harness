#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    /* 1. Non‑deterministic inputs */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    void *array_a;
    void *array_b;

    if (len_a == 0) {
        array_a = NULL;
    } else {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b == 0) {
        array_b = NULL;
    } else {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 2. Pre‑condition assumptions (readability) */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    /* 3. Save old state for immutability checks */
    void *old_array_a = array_a;
    size_t old_len_a = len_a;
    void *old_array_b = array_b;
    size_t old_len_b = len_b;

    void *array_a_copy = NULL;
    void *array_b_copy = NULL;

    if (len_a > 0) {
        array_a_copy = malloc(len_a);
        __CPROVER_assume(array_a_copy != NULL);
        memcpy(array_a_copy, array_a, len_a);
    }
    if (len_b > 0) {
        array_b_copy = malloc(len_b);
        __CPROVER_assume(array_b_copy != NULL);
        memcpy(array_b_copy, array_b, len_b);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Post‑condition: result matches logical equality */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }
    assert(result == expected);

    /* 6. Unchanged input fields */
    assert(array_a == old_array_a);
    assert(len_a == old_len_a);
    assert(array_b == old_array_b);
    assert(len_b == old_len_b);

    /* 7. Input memory must remain unchanged */
    if (len_a > 0) {
        assert_bytes_match((const uint8_t *)array_a, (const uint8_t *)array_a_copy, len_a);
    }
    if (len_b > 0) {
        assert_bytes_match((const uint8_t *)array_b, (const uint8_t *)array_b_copy, len_b);
    }

    /* 8. Clean up */
    free(array_a);
    free(array_b);
    free(array_a_copy);
    free(array_b_copy);
}
