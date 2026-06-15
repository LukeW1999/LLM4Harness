#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    /* Non-deterministic inputs */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    void *array_a = NULL;
    void *array_b = NULL;

    /* Bound and allocate array_a if len_a > 0 */
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }

    /* Bound and allocate array_b if len_b > 0 */
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    /* Save old contents for immutability check */
    struct store_byte_from_buffer storage_a, storage_b;
    if (len_a > 0) {
        save_byte_from_array((const uint8_t *)array_a, len_a, &storage_a);
    }
    if (len_b > 0) {
        save_byte_from_array((const uint8_t *)array_b, len_b, &storage_b);
    }

    /* Call function */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions */
    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        assert(result == true);
    } else {
        /* len_a == len_b > 0 */
        if (result == true) {
            /* Contents are equivalent */
            assert_bytes_match((const uint8_t *)array_a, (const uint8_t *)array_b, len_a);
        } else {
            /* Contents differ */
            /* We cannot assert specific bytes differ, but we can assert the arrays are not equal */
            /* No additional assertion needed beyond the return value */
        }
    }

    /* Immutability: arrays are not modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_a, &storage_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_b, &storage_b);
    }

    /* Cleanup */
    if (len_a > 0) {
        free(array_a);
    }
    if (len_b > 0) {
        free(array_b);
    }
}
