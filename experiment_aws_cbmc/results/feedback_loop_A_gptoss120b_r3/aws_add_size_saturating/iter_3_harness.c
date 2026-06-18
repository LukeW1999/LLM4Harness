#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result_storage = (size_t)nondet_uint64_t();
    size_t old_result = result_storage;
    size_t *result = &result_storage;

    int rc = aws_add_size_saturating(a, b, result);

    if (rc == 0) {
        assert(*result == old_a + old_b);
        assert(old_a <= SIZE_MAX - old_b);
    } else {
        assert(*result == old_result);
        assert(old_a > SIZE_MAX - old_b);
    }
}
