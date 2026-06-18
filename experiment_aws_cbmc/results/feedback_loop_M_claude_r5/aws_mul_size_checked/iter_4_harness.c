#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    int result = aws_mul_size_checked(a, b, &r);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "result is success or error");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a * b, "r equals a * b on success");
    }
}
