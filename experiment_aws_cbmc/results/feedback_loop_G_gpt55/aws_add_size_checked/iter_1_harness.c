#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();
    size_t old_r = r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    int result = aws_add_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(a <= SIZE_MAX - b);
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > SIZE_MAX - b);
        assert(r == old_r);
    }

    assert(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));
}
