#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result = aws_add_size_saturating(a, b);

    if (a <= SIZE_MAX - b) {
        assert(result == a + b);
    } else {
        assert(result == SIZE_MAX);
    }
}
