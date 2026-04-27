#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/common.h"
#include "aws/common/math.h"

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t *r_ptr = &r;

    /* 1. Declare data structure(s) on stack */
    /* No additional data structures needed for this function */

    /* 2. Bound the structure (limits CBMC state space) */
    /* No bounding needed for primitive types */

    /* 3. Allocate pointer members */
    /* No pointer members to allocate */

    /* 4. Assume validity precondition */
    /* No validity preconditions for primitive types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for primitive types */

    /* 6. Assume function-specific preconditions */
    /* No specific preconditions other than those checked within the function */

    /* 7. Call function under test */
    int result = aws_add_u64_checked(a, b, r_ptr);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else if (result == AWS_OP_ERR) {
        assert(a > UINT64_MAX - b);
    }
    /* No state to check for immutability as we're dealing with primitive types */
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t *r_ptr = &r;

    /* 1. Declare data structure(s) on stack */
    /* No additional data structures needed for this function */

    /* 2. Bound the structure (limits CBMC state space) */
    /* No bounding needed for primitive types */

    /* 3. Allocate pointer members */
    /* No pointer members to allocate */

    /* 4. Assume validity precondition */
    /* No validity preconditions for primitive types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for primitive types */

    /* 6. Assume function-specific preconditions */
    /* No specific preconditions other than those checked within the function */

    /* 7. Call function under test */
    int result = aws_add_u32_checked(a, b, r_ptr);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else if (result == AWS_OP_ERR) {
        assert(a > UINT32_MAX - b);
    }
    /* No state to check for immutability as we're dealing with primitive types */
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 1. Declare data structure(s) on stack */
    /* No additional data structures needed for this function */

    /* 2. Bound the structure (limits CBMC state space) */
    /* No bounding needed for primitive types */

    /* 3. Allocate pointer members */
    /* No pointer members to allocate */

    /* 4. Assume validity precondition */
    /* No validity preconditions for primitive types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for primitive types */

    /* 6. Assume function-specific preconditions */
    /* No specific preconditions other than those checked within the function */

    /* 7. Call function under test */
    int result = aws_add_size_checked(a, b, r_ptr);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else if (result == AWS_OP_ERR) {
#if SIZE_BITS == 32
        assert(a > UINT32_MAX - b);
#elif SIZE_BITS == 64
        assert(a > UINT64_MAX - b);
#endif
    }
    /* No state to check for immutability as we're dealing with primitive types */
}
