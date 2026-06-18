#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_val = nondet_size_t();
    size_t old_r_val = r_val;

    int result = aws_add_size_checked(a, b, &r_val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r_val == a + b);
        assert(a <= SIZE_MAX - b);
    } else {
        assert(a > SIZE_MAX - b);
        assert(r_val == old_r_val);
    }
}
