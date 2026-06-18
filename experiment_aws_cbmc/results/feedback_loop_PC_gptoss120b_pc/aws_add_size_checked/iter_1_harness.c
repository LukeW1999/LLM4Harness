#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
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

    /* snapshot inputs for frame condition checks */
    size_t old_a = a;
    size_t old_b = b;

    /* call the function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* result must be one of the defined return codes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* on success the sum must be stored in *r */
        assert(r == a + b);
    } else {
        /* on overflow the output pointer must remain unchanged */
        assert(r == old_r);
    }

    /* frame conditions: inputs must not be modified */
    assert(a == old_a);
    assert(b == old_b);
}
