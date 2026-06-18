#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int ret = aws_mul_size_checked(a, b, &result);

    unsigned __int128 expected = (unsigned __int128)a * b;
    size_t max = (size_t)-1;

    if (expected <= (unsigned __int128)max) {
        assert(ret == 0);
        assert(result == (size_t)expected);
    } else {
        assert(ret != 0);
        assert(result == old_result);
    }
}
