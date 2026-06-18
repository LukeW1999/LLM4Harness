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

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));

        for (size_t i = 0; i < len_a; ++i) {
            array_a[i] = nondet_uint8_t();
        }
    } else if (nondet_bool()) {
        array_a = malloc(1);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));

        for (size_t i = 0; i < len_b; ++i) {
            array_b[i] = nondet_uint8_t();
        }
    } else if (nondet_bool()) {
        array_b = malloc(1);
        __CPROVER_assume(array_b != NULL);
    }

    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    size_t old_len_a = len_a;
    size_t old_len_b = len_b;
    uint8_t *old_ptr_a = array_a;
    uint8_t *old_ptr_b = array_b;

    uint8_t *old_array_a = NULL;
    uint8_t *old_array_b = NULL;

    if (len_a > 0) {
        old_array_a = malloc(len_a);
        __CPROVER_assume(old_array_a != NULL);

        for (size_t i = 0; i < len_a; ++i) {
            old_array_a[i] = array_a[i];
        }
    }

    if (len_b > 0) {
        old_array_b = malloc(len_b);
        __CPROVER_assume(old_array_b != NULL);

        for (size_t i = 0; i < len_b; ++i) {
            old_array_b[i] = array_b[i];
        }
    }

    bool expected = false;

    if (len_a == len_b) {
        expected = true;

        for (size_t i = 0; i < len_a; ++i) {
            if (array_a[i] != array_b[i]) {
                expected = false;
            }
        }
    }

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    assert(result == expected);

    if (result) {
        assert(len_a == len_b);

        for (size_t i = 0; i < len_a; ++i) {
            assert(array_a[i] == array_b[i]);
        }
    } else {
        assert(!expected);

        if (len_a != len_b) {
            assert(len_a != len_b);
        } else {
            bool mismatch_found = false;

            for (size_t i = 0; i < len_a; ++i) {
                if (array_a[i] != array_b[i]) {
                    mismatch_found = true;
                }
            }

            assert(mismatch_found);
        }
    }

    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
    assert(array_a == old_ptr_a);
    assert(array_b == old_ptr_b);

    if (len_a > 0) {
        assert(array_a != NULL);
        assert(old_array_a != NULL);

        for (size_t i = 0; i < len_a; ++i) {
            assert(array_a[i] == old_array_a[i]);
        }
    }

    if (len_b > 0) {
        assert(array_b != NULL);
        assert(old_array_b != NULL);

        for (size_t i = 0; i < len_b; ++i) {
            assert(array_b[i] == old_array_b[i]);
        }
    }

    assert((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    assert((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));
}
