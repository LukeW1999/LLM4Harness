#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

void harness() {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    int overflow = (a != 0) && (b > SIZE_MAX / a);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a * b, "r equals a*b on success");
        __CPROVER_assert(!overflow, "no overflow on success");
    } else {
        __CPROVER_assert(overflow, "overflow on failure");
    }
}
