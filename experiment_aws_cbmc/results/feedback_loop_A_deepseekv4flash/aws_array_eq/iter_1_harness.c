#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    const void *array_a = NULL;
    const void *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    // Preconditions from the implementation:
    // (len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a)
    // (len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b)
    // These are satisfied by our allocation.

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // Postcondition: result is true iff lengths are equal and,
    // if len > 0, the contents compare equal.
    bool expected = (len_a == len_b) && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0);
    assert(result == expected);
}
