#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a;
    size_t b;

    // Precondition: none (inputs are arbitrary size_t values)

    size_t result = aws_add_size_saturating(a, b);

    // Postcondition: if addition would overflow, result is SIZE_MAX; otherwise result == a + b
    if (a > SIZE_MAX - b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }

    // Ensure both branches are reachable by providing a and b that cover both cases.
    // CBMC will explore both possibilities due to nondeterministic inputs.
}
