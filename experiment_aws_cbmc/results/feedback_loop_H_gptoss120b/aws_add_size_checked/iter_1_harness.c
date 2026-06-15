/* Harness for aws_add_u64_checked, aws_add_u32_checked, and aws_add_size_checked */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/* Include the AWS headers that declare the functions under test */
#include "aws/common/math.h"

/* Proof helper declarations (normally provided by the verification framework) */
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);
uint32_t nondet_uint32_t(void);
uint64_t nondet_uint64_t(void);
size_t nondet_size_t(void);

/* -------------------------------------------------------------------------- */
/* Harness for aws_add_u64_checked                                            */
/* -------------------------------------------------------------------------- */
void aws_add_u64_checked_harness(void) {
    /* 1. Non‑deterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* 2. Allocate output pointer */
    uint64_t *r = (uint64_t *)malloc(sizeof(uint64_t));
    __CPROVER_assume(r != NULL);               /* pointer must be valid */

    /* 3. Save old state */
    uint64_t old_r_value = *r;
    uint64_t *old_r_ptr   = r;
    uint64_t old_a        = a;
    uint64_t old_b        = b;

    /* 4. Call the function under test */
    int result = aws_add_u64_checked(a, b, r);

    /* 5. Post‑condition for the success path */
    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        /* 6. Post‑condition for the failure path */
        assert(result == AWS_OP_ERR);
        assert(*r == old_r_value);            /* output unchanged on error */
    }

    /* 7. Fields that must never change */
    assert(r == old_r_ptr);
    assert(a == old_a);
    assert(b == old_b);

    /* 8. Clean up */
    free(r);
}

/* -------------------------------------------------------------------------- */
/* Harness for aws_add_u32_checked                                            */
/* -------------------------------------------------------------------------- */
void aws_add_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t *r = (uint32_t *)malloc(sizeof(uint32_t));
    __CPROVER_assume(r != NULL);

    uint32_t old_r_value = *r;
    uint32_t *old_r_ptr   = r;
    uint32_t old_a        = a;
    uint32_t old_b        = b;

    int result = aws_add_u32_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == old_r_value);
    }

    assert(r == old_r_ptr);
    assert(a == old_a);
    assert(b == old_b);

    free(r);
}

/* -------------------------------------------------------------------------- */
/* Harness for aws_add_size_checked                                          */
/* -------------------------------------------------------------------------- */
void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t *r = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    size_t old_r_value = *r;
    size_t *old_r_ptr   = r;
    size_t old_a        = a;
    size_t old_b        = b;

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == old_r_value);
    }

    assert(r == old_r_ptr);
    assert(a == old_a);
    assert(b == old_b);

    free(r);
}
