#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();
    size_t old_r = r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}
