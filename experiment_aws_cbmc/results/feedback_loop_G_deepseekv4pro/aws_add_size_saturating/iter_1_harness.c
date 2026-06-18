#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness() {
    /* Input values */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* The function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* Postcondition: result is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Partition on success or error */
    if (result == AWS_OP_SUCCESS) {
        /* No overflow: r must equal a + b */
        assert(r == a + b);
        /* Explicit no‑overflow condition */
        assert(a <= SIZE_MAX - b);
    } else {
        /* Overflow must have occurred */
        assert(a > SIZE_MAX - b);
    }
}
