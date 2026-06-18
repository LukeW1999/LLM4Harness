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

    size_t allocation_size = MAX_BUFFER_SIZE == 0 ? 1 : MAX_BUFFER_SIZE;

    uint8_t *backing_a = malloc(allocation_size);
    uint8_t *backing_b = malloc(allocation_size);
    uint8_t *old_a_contents = malloc(allocation_size);
    uint8_t *old_b_contents = malloc(allocation_size);

    __CPROVER_assume(backing_a != NULL);
    __CPROVER_assume(backing_b != NULL);
    __CPROVER_assume(old_a_contents != NULL);
    __CPROVER_assume(old_b_contents != NULL);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        backing_a[i] = nondet_uint8_t();
        backing_b[i] = nondet_uint8_t();
    }

    const uint8_t *array_a = backing_a;
    const uint8_t *array_b = nondet_bool() ? backing_a : backing_b;

    if (len_a == 0 && nondet_bool()) {
        array_a = NULL;
    }

    if (len_b == 0 && nondet_bool()) {
        array_b = NULL;
    }

    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    const uint8_t *old_array_a = array_a;
    const uint8_t *old_array_b = array_b;
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    for (size_t i = 0; i < len_a; ++i) {
        old_a_contents[i] = array_a[i];
    }

    for (size_t i = 0; i < len_b; ++i) {
        old_b_contents[i] = array_b[i];
    }

    bool expected_result = (len_a == len_b);
    if (expected_result) {
        for (size_t i = 0; i < len_a; ++i) {
            if (array_a[i] != array_b[i]) {
                expected_result = false;
            }
        }
    }

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    assert(array_a == old_array_a);
    assert(array_b == old_array_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    assert((array_a != NULL) || (len_a == 0));
    assert((array_b != NULL) || (len_b == 0));
    assert((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    assert((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    for (size_t i = 0; i < len_a; ++i) {
        assert(array_a[i] == old_a_contents[i]);
    }

    for (size_t i = 0; i < len_b; ++i) {
        assert(array_b[i] == old_b_contents[i]);
    }

    assert(result == expected_result);

    if (result) {
        assert(len_a == len_b);
        for (size_t i = 0; i < len_a; ++i) {
            assert(array_a[i] == array_b[i]);
        }
    } else {
        assert(!expected_result);
        if (len_a == len_b) {
            bool found_mismatch = false;
            for (size_t i = 0; i < len_a; ++i) {
                if (array_a[i] != array_b[i]) {
                    found_mismatch = true;
                }
            }
            assert(found_mismatch);
        } else {
            assert(len_a != len_b);
        }
    }
}
