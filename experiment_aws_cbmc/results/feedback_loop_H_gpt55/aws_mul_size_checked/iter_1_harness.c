#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_varargs_harness() {
    size_t num = nondet_size_t();
    size_t r = nondet_size_t();

    size_t a0 = nondet_size_t();
    size_t a1 = nondet_size_t();
    size_t a2 = nondet_size_t();
    size_t a3 = nondet_size_t();

    __CPROVER_assume(num <= 4);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    bool overflow = false;
    size_t expected = 0;

    if (num > 0) {
        if (a0 > SIZE_MAX - expected) {
            overflow = true;
        } else {
            expected += a0;
        }
    }

    if (!overflow && num > 1) {
        if (a1 > SIZE_MAX - expected) {
            overflow = true;
        } else {
            expected += a1;
        }
    }

    if (!overflow && num > 2) {
        if (a2 > SIZE_MAX - expected) {
            overflow = true;
        } else {
            expected += a2;
        }
    }

    if (!overflow && num > 3) {
        if (a3 > SIZE_MAX - expected) {
            overflow = true;
        } else {
            expected += a3;
        }
    }

    int result = aws_add_size_checked_varargs(num, &r, a0, a1, a2, a3);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == expected);
    }

    assert(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));
}
