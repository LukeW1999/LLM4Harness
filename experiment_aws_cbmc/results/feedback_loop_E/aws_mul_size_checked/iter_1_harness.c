#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();
    size_t *r_ptr = &r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(*r_ptr)));

    int result = aws_mul_size_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(*r_ptr == a * b);
    } else {
        assert(*r_ptr == SIZE_MAX);
        assert(a > 0 && b > SIZE_MAX / a);
    }

    assert(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(*r_ptr)));
}
