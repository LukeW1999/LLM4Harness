#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <aws/common/math.h>
#include <proof_helpers/nondet.h>

/* Harness for aws_add_u64_checked */
void aws_add_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t *r = malloc(sizeof(uint64_t));
    __CPROVER_assume(r != NULL);

    uint64_t old_r = *r;

    int result = aws_add_u64_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* No overflow occurred */
        assert((b == 0) || (a <= UINT64_MAX - b));
        /* Result is the sum */
        assert(*r == a + b);
    } else {
        /* Overflow detected */
        assert((b > 0) && (a > UINT64_MAX - b));
        /* Output pointer must remain unchanged */
        assert(*r == old_r);
    }

    /* Pointer itself must remain valid */
    assert(r != NULL);
    free(r);
}

/* Harness for aws_add_u32_checked */
void aws_add_u32_checked_harness(void) {
    uint32_t a = (uint32_t)nondet_uint32_t();
    uint32_t b = (uint32_t)nondet_uint32_t();

    uint32_t *r = malloc(sizeof(uint32_t));
    __CPROVER_assume(r != NULL);

    uint32_t old_r = *r;

    int result = aws_add_u32_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* No overflow occurred */
        assert((b == 0) || (a <= UINT32_MAX - b));
        /* Result is the sum */
        assert(*r == a + b);
    } else {
        /* Overflow detected */
        assert((b > 0) && (a > UINT32_MAX - b));
        /* Output pointer must remain unchanged */
        assert(*r == old_r);
    }

    /* Pointer itself must remain valid */
    assert(r != NULL);
    free(r);
}

/* Harness for aws_add_size_checked */
void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    size_t old_r = *r;

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
#if SIZE_BITS == 32
        assert((b == 0) || (a <= UINT32_MAX - b));
#else
        assert((b == 0) || (a <= UINT64_MAX - b));
#endif
        assert(*r == a + b);
    } else {
#if SIZE_BITS == 32
        assert((b > 0) && (a > UINT32_MAX - b));
#else
        assert((b > 0) && (a > UINT64_MAX - b));
#endif
        assert(*r == old_r);
    }

    /* Pointer itself must remain valid */
    assert(r != NULL);
    free(r);
}
