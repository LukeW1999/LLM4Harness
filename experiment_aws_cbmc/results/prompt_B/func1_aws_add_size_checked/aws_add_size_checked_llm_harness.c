#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/proof_allocators.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = can_fail_malloc(sizeof(*r));

    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    size_t r_old = *r;

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else if (result == AWS_OP_ERR) {
        assert(*r == r_old); // Ensure r is not modified on error
    } else {
        assert(0); // Unreachable, as per function specification
    }
}
