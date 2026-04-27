#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_harness() {
    // 1. Declare and bound data structures
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    uint8_t *array_a = can_fail_malloc(len_a);
    uint8_t *array_b = can_fail_malloc(len_b);

    __CPROVER_assume(array_a != NULL);
    __CPROVER_assume(array_b != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));

    // Add assumptions to ensure memcmp preconditions are met
    __CPROVER_assume(len_a == len_b);

    // 3. Call function under test
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // 4. Assert postconditions for BOTH success and failure paths
    if (result) {
        assert(len_a == len_b);
        if (len_a > 0) {
            assert(!memcmp(array_a, array_b, len_a));
        }
    } else {
        assert(len_a != len_b || (len_a > 0 && memcmp(array_a, array_b, len_a) != 0));
    }
}
