#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();
    size_t old_n = n;

    /* call the function under test */
    bool result = aws_is_power_of_two(n);

    /* frame condition: input unchanged */
    assert(n == old_n);

    /* validity predicate */
    if (result) {
        /* result true ⇒ n is a non‑zero power of two */
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    } else {
        /* result false ⇒ n is zero or not a power of two */
        assert((n == 0) || ((n & (n - 1)) != 0));
    }
}
