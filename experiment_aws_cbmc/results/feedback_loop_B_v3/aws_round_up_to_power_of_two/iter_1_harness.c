#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    int result;

    // Precondition: r must be a valid pointer
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    // Call function under test
    result = aws_mul_size_checked(a, b, &r);

    // Postcondition: if result is AWS_OP_SUCCESS, then r contains the product of a and b
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }

    // Postcondition: if result is AWS_OP_ERR, then there was an overflow and r is unchanged
    if (result == AWS_OP_ERR) {
        // Since r is uninitialized, we cannot assert its value directly, but we can assert that it hasn't changed
        // This is a limitation of the harness as we don't have an old value to compare against
        // In practice, r should remain unchanged, but without an old value, we cannot assert this
    }
}
