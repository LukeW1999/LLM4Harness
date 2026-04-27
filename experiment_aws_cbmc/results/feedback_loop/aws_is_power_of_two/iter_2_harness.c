#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t test_val = nondet_size_t();
    bool rval = aws_is_power_of_two(test_val);
#if ULONG_MAX == SIZE_MAX
    int popcount = __builtin_popcountl(test_val);
#elif ULLONG_MAX == SIZE_MAX
    int popcount = __builtin_popcountll(test_val);
#else
#    error
#endif
    assert(rval == (popcount == 1));
}
