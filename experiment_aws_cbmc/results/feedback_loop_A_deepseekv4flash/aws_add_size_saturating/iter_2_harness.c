#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a;
    size_t b;

    // Call the function
    size_t result = aws_add_size_saturating(a, b);

    // Postcondition: if addition would overflow, result is SIZE_MAX; otherwise result == a + b
    if (a > SIZE_MAX - b) {
        // overflow occurs
        assert(result == SIZE_MAX);
    } else {
        // no overflow
        assert(result == a + b);
    }

    // Unchanged: a and b are not modified (passed by value)
}
