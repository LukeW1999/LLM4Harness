#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Coverage: aws_add_size_checked
 *
 * Assumptions:
 *     - given 2 non-deterministic size_t integers
 *     - r is a valid writable pointer
 *
 * Assertions:
 *     - if AWS_OP_SUCCESS: r == a + b (no overflow occurred)
 *     - if AWS_OP_ERR: overflow condition holds (b > 0 && a > SIZE_MAX - b)
 */
void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    int rval = aws_add_size_checked(a, b, &r);

    if (rval == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(rval == AWS_OP_ERR);
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }
}
