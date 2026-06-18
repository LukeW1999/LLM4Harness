#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_is_power_of_two_harness(void) {
    /* Non‑deterministic input, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= 1024);

    /* Call the function under test */
    bool result = aws_is_power_of_two(n);

    /* Post‑condition checks */
    if (result) {
        /* n must be a non‑zero power of two */
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    } else {
        /* n is either zero or not a power of two */
        assert(!(n != 0 && (n & (n - 1)) == 0));
    }
}
