#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;
    struct store_byte_from_buffer saved_a;
    struct store_byte_from_buffer saved_b;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        save_byte_from_array(array_a, len_a, &saved_a);
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        save_byte_from_array(array_b, len_b, &saved_b);
    }

    bool result = aws_array_eq((const void *)array_a, len_a, (const void *)array_b, len_b);

    // Expected result according to the specification
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }
    assert(result == expected);

    // Ensure arrays were not modified
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &saved_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &saved_b);
    }

    // Direct byte-wise check if the result is true and lengths are positive
    if (result && len_a > 0) {
        for (size_t i = 0; i < len_a; i++) {
            assert(array_a[i] == array_b[i]);
        }
    }
}
