#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;
    size_t old_r = r;

    bool overflow = (b > 0) && (a > (SIZE_MAX - b));

    int result = aws_add_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow) {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a + b);
    }

    assert(a == old_a);
    assert(b == old_b);
    assert(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));
}
