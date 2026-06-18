#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* output pointer with nondeterministic initial value */
    size_t r = nondet_size_t();
    size_t old_r = r;

    /* call the function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* result must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* on success the sum must be stored and no overflow occurred */
        assert(r == a + b);
        assert(!(b > 0 && a > (SIZE_MAX - b)));
    } else {
        /* on failure the output must be unchanged and overflow must have been detected */
        assert(r == old_r);
        assert(b > 0 && a > (SIZE_MAX - b));
    }
}
