#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_array_eq_harness(void) {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = aws_mem_acquire(allocator, len_a);
        __CPROVER_assume(array_a != NULL);
    }
    if (len_b > 0) {
        array_b = aws_mem_acquire(allocator, len_b);
        __CPROVER_assume(array_b != NULL);
    }

    struct store_byte_from_buffer old_a, old_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_b);
    }

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    if (result) {
        __CPROVER_assert(len_a == len_b, "Lengths must be equal when arrays are reported equal");
        if (len_a > 0) {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        if (len_a == len_b && len_a > 0) {
            __CPROVER_assert(memcmp(array_a, array_b, len_a) != 0,
                             "Arrays must differ when reported not equal");
        }
    }

    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &old_a);
        __CPROVER_assert(array_a == old_a.buffer, "array_a pointer unchanged");
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &old_b);
        __CPROVER_assert(array_b == old_b.buffer, "array_b pointer unchanged");
    }

    if (array_a) {
        aws_mem_release(allocator, array_a);
    }
    if (array_b) {
        aws_mem_release(allocator, array_b);
    }
}
