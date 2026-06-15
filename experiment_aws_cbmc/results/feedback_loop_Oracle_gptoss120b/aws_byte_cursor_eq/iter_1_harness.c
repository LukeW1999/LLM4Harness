#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_eq_harness(void) {
    struct aws_byte_cursor lhs, rhs;
    bool result;

    /* ----- lhs setup ----- */
    lhs.len = nondet_size_t();
    __CPROVER_assume(lhs.len <= MAX_BUFFER_SIZE);
    if (lhs.len > 0) {
        lhs.ptr = malloc(lhs.len);
        __CPROVER_assume(lhs.ptr != NULL);
        for (size_t i = 0; i < lhs.len; ++i) {
            lhs.ptr[i] = nondet_uint8_t();
        }
    } else {
        lhs.ptr = NULL;
    }

    /* ----- rhs setup ----- */
    rhs.len = nondet_size_t();
    __CPROVER_assume(rhs.len <= MAX_BUFFER_SIZE);
    if (rhs.len > 0) {
        rhs.ptr = malloc(rhs.len);
        __CPROVER_assume(rhs.ptr != NULL);
        for (size_t i = 0; i < rhs.len; ++i) {
            rhs.ptr[i] = nondet_uint8_t();
        }
    } else {
        rhs.ptr = NULL;
    }

    /* ----- structural assumptions ----- */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&lhs));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&rhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&rhs));

    /* ----- snapshot for frame condition ----- */
    struct aws_byte_cursor lhs_old = lhs;
    struct aws_byte_cursor rhs_old = rhs;

    uint8_t *lhs_copy = NULL;
    uint8_t *rhs_copy = NULL;

    if (lhs.len > 0) {
        lhs_copy = malloc(lhs.len);
        __CPROVER_assume(lhs_copy != NULL);
        memcpy(lhs_copy, lhs.ptr, lhs.len);
    }
    if (rhs.len > 0) {
        rhs_copy = malloc(rhs.len);
        __CPROVER_assume(rhs_copy != NULL);
        memcpy(rhs_copy, rhs.ptr, rhs.len);
    }

    /* ----- call under test ----- */
    result = aws_byte_cursor_eq(&lhs, &rhs);

    /* ----- post‑condition: return value correctness ----- */
    bool expected;
    if (lhs.len != rhs.len) {
        expected = false;
    } else if (lhs.len == 0) {
        expected = true;
    } else {
        expected = (memcmp(lhs.ptr, rhs.ptr, lhs.len) == 0);
    }
    assert(result == expected);

    /* ----- post‑condition: frame condition (no memory modification) ----- */
    assert(lhs.len == lhs_old.len);
    assert(lhs.ptr == lhs_old.ptr);
    if (lhs.len > 0) {
        assert(memcmp(lhs.ptr, lhs_copy, lhs.len) == 0);
    }

    assert(rhs.len == rhs_old.len);
    assert(rhs.ptr == rhs_old.ptr);
    if (rhs.len > 0) {
        assert(memcmp(rhs.ptr, rhs_copy, rhs.len) == 0);
    }

    /* clean up */
    free(lhs.ptr);
    free(rhs.ptr);
    free(lhs_copy);
    free(rhs_copy);

    return 0;
}
