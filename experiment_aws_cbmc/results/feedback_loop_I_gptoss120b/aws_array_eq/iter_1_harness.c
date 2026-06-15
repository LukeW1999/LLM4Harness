#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        for (size_t i = 0; i < len_a; ++i) {
            array_a[i] = nondet_uint8_t();
        }
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            array_b[i] = nondet_uint8_t();
        }
    }

    /* 2. Save old state */
    const void *old_array_a = array_a;
    const void *old_array_b = array_b;
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Post‑condition: result matches specification */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }
    assert(result == expected);

    /* 5. Unchanged inputs */
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
}
