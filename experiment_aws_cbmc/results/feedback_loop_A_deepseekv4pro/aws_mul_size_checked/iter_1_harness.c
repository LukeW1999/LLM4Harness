#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = nondet_size_t();
    r = old_r;

    /* r must point to writable memory */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(size_t)));

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* No overflow: r must hold the exact sum */
        assert(r == a + b);
        /* The addition must not overflow */
        assert(a <= SIZE_MAX - b);
    } else {
        /* Overflow: r must remain unchanged */
        assert(r == old_r);
        /* The addition must overflow */
        assert(a > SIZE_MAX - b);
    }
}
