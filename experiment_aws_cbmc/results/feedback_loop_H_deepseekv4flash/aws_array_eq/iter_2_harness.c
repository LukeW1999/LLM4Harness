#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    size_t len_a;
    size_t len_b;
    void *array_a = NULL;
    void *array_b = NULL;

    len_a = nondet_size_t();
    len_b = nondet_size_t();

    /* Bound lengths to avoid exhaustive allocation */
    __CPROVER_assume(len_a <= MAX_MALLOC_SIZE);
    __CPROVER_assume(len_b <= MAX_MALLOC_SIZE);

    /* Allocate and initialize memory if lengths are positive */
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    /* Save original content for later comparison */
    struct store_byte_from_buffer storage_a, storage_b;
    if (len_a > 0) {
        save_byte_from_array((const uint8_t *)array_a, len_a, &storage_a);
    }
    if (len_b > 0) {
        save_byte_from_array((const uint8_t *)array_b, len_b, &storage_b);
    }

    /* Call the function under verification */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Assert postconditions */
    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        assert(result == true);
    } else {
        /* len_a == len_b > 0 */
        if (result == true) {
            assert_bytes_match((const uint8_t *)array_a, (const uint8_t *)array_b, len_a);
        } else {
            /* At least one pair of bytes differs */
            assert(!assert_bytes_match((const uint8_t *)array_a, (const uint8_t *)array_b, len_a));
        }
    }

    /* Immutability: arrays are not modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_a, &storage_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array_b, &storage_b);
    }

    /* Clean up allocated memory */
    if (len_a > 0) {
        free(array_a);
    }
    if (len_b > 0) {
        free(array_b);
    }
}
