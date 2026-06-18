#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = (size_t)nondet_uint64_t();

    /* nondeterministic initial result value */
    size_t result_val = (size_t)nondet_uint64_t();
    size_t *result = &result_val;

    /* save old state */
    size_t old_n = n;
    size_t old_val = *result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(*result));
        assert(*result >= old_n);
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        assert(ret == AWS_OP_ERR);
        assert(*result == old_val);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);
}
