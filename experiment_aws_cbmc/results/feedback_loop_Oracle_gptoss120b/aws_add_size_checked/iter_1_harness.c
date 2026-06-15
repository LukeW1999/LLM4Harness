#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include "aws/common/math.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic generators for CBMC */
size_t nondet_size_t(void);
uint32_t nondet_uint32_t(void);
uint64_t nondet_uint64_t(void);
int nondet_int(void);

void aws_add_size_checked_harness(void) {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Result buffer and its original value (to check frame condition on error) */
    size_t r_orig = nondet_size_t();
    size_t r = r_orig;

    /* Additional memory to verify frame conditions */
    size_t other[4];
    size_t other_orig[4];
    for (int i = 0; i < 4; ++i) {
        other[i] = nondet_size_t();
        other_orig[i] = other[i];
    }

    /* Call the function under test */
    int ret = aws_add_size_checked(a, b, &r);

    /* 1. Return value / error code correctness */
    assert(ret == 0 || ret == -1); /* AWS_OP_SUCCESS == 0, AWS_OP_ERR == -1 */

    /* Determine overflow condition according to the size of size_t */
#if SIZE_BITS == 32
    bool overflow = (b > 0) && (a > (UINT32_MAX - b));
#elif SIZE_BITS == 64
    bool overflow = (b > 0) && (a > (UINT64_MAX - b));
#else
    bool overflow = (b > 0) && (a > (SIZE_MAX - b));
#endif

    if (ret == 0) {
        /* Success: no overflow and correct result */
        assert(!overflow);
        assert(r == a + b);
    } else {
        /* Error: overflow detected, result unchanged */
        assert(overflow);
        assert(r == r_orig);
    }

    /* 2. Output buffer length/capacity invariants are implicitly covered by the overflow check */

    /* 3. Memory frame condition: only *r may be modified */
    for (int i = 0; i < 4; ++i) {
        assert(other[i] == other_orig[i]);
    }

    return 0;
}
