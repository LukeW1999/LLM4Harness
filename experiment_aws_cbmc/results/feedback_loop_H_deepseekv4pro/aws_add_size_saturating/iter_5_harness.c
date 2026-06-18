#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void harness() {
    uint32_t a;
    uint32_t b;
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert((uint64_t)a + (uint64_t)b == r);
        assert(a <= UINT32_MAX - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert((uint64_t)a + (uint64_t)b > UINT32_MAX);
    }
}
