#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* Assume bounds for array sizes */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically */
    void *array_a = malloc(len_a ? len_a : 1);
    void *array_b = malloc(len_b ? len_b : 1);

    /* If length is 0, array may be NULL */
    if (len_a == 0) {
        __CPROVER_assume(array_a == NULL || AWS_MEM_IS_READABLE(array_a, 0));
    } else {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (len_b == 0) {
        __CPROVER_assume(array_b == NULL || AWS_MEM_IS_READABLE(array_b, 0));
    } else {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    /* Save old arrays if both are readable */
    struct store_byte_from_buffer stored_a;
    struct store_byte_from_buffer stored_b;
    if (len_a > 0 && array_a != NULL) {
        save_byte_from_array((const uint8_t *)array_a, len_a, &stored_a);
    }
    if (len_b > 0 && array_b != NULL) {
        save_byte_from_array((const uint8_t *)array_b, len_b, &stored_b);
    }

    /* Call function */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions based on specification and implementation */

    /* 1. If lengths differ, result is false */
    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        /* Both lengths zero -> equal (both empty) */
        assert(result == true);
    } else {
        /* Both lengths equal and > 0: result depends on memcmp */
        if (array_a != NULL && array_b != NULL) {
            int cmp = memcmp(array_a, array_b, len_a);
            assert(result == (cmp == 0));
        }
    }

    /* 2. Arrays themselves are unchanged (const qualification) */
    if (len_a > 0 && array_a != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)array_a, &stored_a);
    }
    if (len_b > 0 && array_b != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)array_b, &stored_b);
    }

    /* 3. Lengths are unchanged */
    // len_a and len_b are passed by value, so they are not modified

    /* 4. No memory allocation or deallocation occurs */
    // No dynamic memory management in this function
}
