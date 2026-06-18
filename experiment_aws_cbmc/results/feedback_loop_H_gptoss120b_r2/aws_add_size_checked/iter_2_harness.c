#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t r;
    uint64_t old_r = nondet_uint64_t();
    r = old_r;

    int result = aws_add_u64_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }
}
