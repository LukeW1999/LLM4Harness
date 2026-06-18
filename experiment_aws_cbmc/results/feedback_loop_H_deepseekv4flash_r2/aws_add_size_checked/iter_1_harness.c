#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t *r_ptr = &r;

    int result = aws_add_u64_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(*r_ptr == a + b);
    } else {
        assert(result == AWS_OP_ERR);
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t *r_ptr = &r;

    int result = aws_add_u32_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(*r_ptr == a + b);
    } else {
        assert(result == AWS_OP_ERR);
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    int result = aws_add_size_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(*r_ptr == a + b);
    } else {
        assert(result == AWS_OP_ERR);
    }
}
