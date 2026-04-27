#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    /* 1. Declare and bound data structures */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    uint8_t *array_a = can_fail_malloc(len_a);
    uint8_t *array_b = can_fail_malloc(len_b);

    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    if (array_a) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (array_b) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    uint8_t array_a_old[len_a];
    uint8_t array_b_old[len_b];

    if (array_a && len_a > 0) {
        memcpy(array_a_old, array_a, len_a);
    }
    if (array_b && len_b > 0) {
        memcpy(array_b_old, array_b, len_b);
    }

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(len_a == len_b);
        if (len_a > 0) {
            assert(!memcmp(array_a, array_b, len_a));
        }
    } else {
        if (array_a && array_b) {
            assert(memcmp(array_a, array_b, len_a) != 0 || len_a != len_b);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    if (array_a && len_a > 0) {
        assert_bytes_match(array_a, array_a_old, len_a);
    }
    if (array_b && len_b > 0) {
        assert_bytes_match(array_b, array_b_old, len_b);
    }

    /* 6. Assert validity invariants always holds */
    if (array_a) {
        assert(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (array_b) {
        assert(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    /* Additional assumptions to ensure preconditions are met */
    if (array_a) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (array_b) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }
}
