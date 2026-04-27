#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    // No fields to check for unchanged values as the function only modifies the output parameter r
    // Validity invariants: none specific to the function, as it deals with primitive types
}
