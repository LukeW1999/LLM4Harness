#include <assert.h>
#include <stddef.h>
#include <limits.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    size_t *presult = &result;

    size_t old_a = a;
    size_t old_b = b;

    int ret = aws_add_size_saturating(a, b, presult);

    if (ret == 0) {
        assert(*presult == old_a + old_b);
        assert(SIZE_MAX - old_a >= old_b);
    } else {
        assert(*presult == SIZE_MAX);
        assert(SIZE_MAX - old_a < old_b);
    }
}
