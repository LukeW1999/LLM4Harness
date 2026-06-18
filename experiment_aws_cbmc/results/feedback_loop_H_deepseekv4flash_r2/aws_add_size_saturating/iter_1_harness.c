#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t *r = malloc(sizeof(uint64_t));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(uint64_t)));
    uint64_t old_r = *r;
    int ret = aws_mul_u64_checked(a, b, r);
    if (ret == AWS_OP_SUCCESS) {
        assert(*r == a * b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(*r == old_r);
    }
    free(r);
}
