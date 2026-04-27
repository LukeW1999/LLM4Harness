#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_harness() {
    /* 1. Declare and bound data structures */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    const void *array_a = can_fail_malloc(len_a);
    const void *array_b = can_fail_malloc(len_b);

    __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct {
        const void *array_a;
        const void *array_b;
        size_t len_a;
        size_t len_b;
    } old = {array_a, array_b, len_a, len_b};

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(len_a == len_b);
        assert(!memcmp(array_a, array_b, len_a));
    } else {
        /* No specific changes on failure, just ensure inputs are unchanged */
        assert(array_a == old.array_a);
        assert(array_b == old.array_b);
        assert(len_a == old.len_a);
        assert(len_b == old.len_b);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No fields to assert here as we are dealing with primitive types and pointers */

    /* 6. Assert validity invariants always holds */
    /* No validity invariants to assert here as we are dealing with primitive types and pointers */
}
