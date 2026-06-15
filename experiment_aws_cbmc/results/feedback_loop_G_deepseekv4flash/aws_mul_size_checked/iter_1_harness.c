#include <stddef.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

/* Function under test */
int aws_mul_size_checked(size_t a, size_t b, size_t *r);

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    size_t r_old = *r;

    int ret = aws_mul_size_checked(a, b, r);

    __CPROVER_assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR,
                     "Return must be success or error");

    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(*r == a * b,
                         "On success, *r must equal the product");
    } else {
        __CPROVER_assert(*r == r_old,
                         "On overflow, *r must remain unchanged");
    }
}
