#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Harness for aws_add_size_checked */
void aws_add_size_checked_harness() {
    /* 1. Declare non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* 2. Save old state */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 4. Assert postconditions */

    /* Inputs must not change */
    assert(a == old_a);
    assert(b == old_b);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, and no overflow occurred */
        assert(r == a + b);
        /* Verify no overflow: if b > 0 then a <= SIZE_MAX - b */
        if (b > 0) {
            assert(a <= (SIZE_MAX - b));
        }
        /* r must be >= both a and b (since no overflow) */
        assert(r >= a);
        assert(r >= b);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == -1);
        /* Overflow must have occurred: b > 0 and a > SIZE_MAX - b */
        assert(b > 0);
        assert(a > (SIZE_MAX - b));
        /* r is not written on failure path, so we don't assert r */
    }
}

/* Harness for aws_add_u64_checked */
void aws_add_u64_checked_harness() {
    /* 1. Declare non-deterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    /* 2. Save old state */
    uint64_t old_a = a;
    uint64_t old_b = b;

    /* 3. Call function under test */
    int result = aws_add_u64_checked(a, b, &r);

    /* 4. Assert postconditions */

    /* Inputs must not change */
    assert(a == old_a);
    assert(b == old_b);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, and no overflow occurred */
        assert(r == a + b);
        /* Verify no overflow: if b > 0 then a <= UINT64_MAX - b */
        if (b > 0) {
            assert(a <= (UINT64_MAX - b));
        }
        /* r must be >= both a and b (since no overflow) */
        assert(r >= a);
        assert(r >= b);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == -1);
        /* Overflow must have occurred: b > 0 and a > UINT64_MAX - b */
        assert(b > 0);
        assert(a > (UINT64_MAX - b));
    }
}

/* Harness for aws_add_u32_checked */
void aws_add_u32_checked_harness() {
    /* 1. Declare non-deterministic inputs */
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    /* 2. Save old state */
    uint32_t old_a = a;
    uint32_t old_b = b;

    /* 3. Call function under test */
    int result = aws_add_u32_checked(a, b, &r);

    /* 4. Assert postconditions */

    /* Inputs must not change */
    assert(a == old_a);
    assert(b == old_b);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, and no overflow occurred */
        assert(r == (uint32_t)(a + b));
        /* Verify no overflow: if b > 0 then a <= UINT32_MAX - b */
        if (b > 0) {
            assert(a <= (UINT32_MAX - b));
        }
        /* r must be >= both a and b (since no overflow) */
        assert(r >= a);
        assert(r >= b);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == -1);
        /* Overflow must have occurred: b > 0 and a > UINT32_MAX - b */
        assert(b > 0);
        assert(a > (UINT32_MAX - b));
    }
}
