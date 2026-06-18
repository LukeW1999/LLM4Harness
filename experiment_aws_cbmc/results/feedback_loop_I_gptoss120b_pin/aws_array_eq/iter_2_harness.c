#include <aws/common/array.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
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

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer old_a, old_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_b);
    }

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Postconditions for success and failure paths */
    if (result) {
        /* Success: lengths equal and contents identical */
        assert(len_a == len_b);
        if (len_a > 0) {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        /* Failure: either lengths differ, or lengths equal >0 and contents differ */
        if (len_a == len_b && len_a > 0) {
            assert(memcmp(array_a, array_b, len_a) != 0);
        }
    }

    /* 5. Fields that must not change regardless of result */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &old_a);
        assert(array_a == old_a.buffer);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &old_b);
        assert(array_b == old_b.buffer);
    }

    /* 6. Clean up */
    if (array_a) {
        aws_mem_release(allocator, array_a);
    }
    if (array_b) {
        aws_mem_release(allocator, array_b);
    }
}
