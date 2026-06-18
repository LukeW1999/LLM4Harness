#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    size_t result = (size_t)nondet_uint64_t();

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    int rc = aws_add_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(result == a + b);
    } else {
        assert(result == old_result);
    }

    assert(a == old_a);
    assert(b == old_b);
}
