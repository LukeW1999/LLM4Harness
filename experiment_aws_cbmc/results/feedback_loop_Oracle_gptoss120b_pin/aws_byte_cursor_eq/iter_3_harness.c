#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t();
uint8_t nondet_uint8_t();

void aws_byte_cursor_eq_harness(void) {
    const size_t MAX_BUFFER_SIZE = 64;
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor lhs;
    struct aws_byte_cursor rhs;

    /* nondet length for lhs */
    lhs.len = nondet_size_t();
    __CPROVER_assume(lhs.len <= MAX_BUFFER_SIZE);
    if (lhs.len > 0) {
        lhs.ptr = (uint8_t *)aws_mem_acquire(allocator, lhs.len);
        __CPROVER_assume(lhs.ptr != NULL);
        for (size_t i = 0; i < lhs.len; ++i) {
            lhs.ptr[i] = nondet_uint8_t();
        }
    } else {
        lhs.ptr = NULL;
    }

    /* nondet length for rhs */
    rhs.len = nondet_size_t();
    __CPROVER_assume(rhs.len <= MAX_BUFFER_SIZE);
    if (rhs.len > 0) {
        rhs.ptr = (uint8_t *)aws_mem_acquire(allocator, rhs.len);
        __CPROVER_assume(rhs.ptr != NULL);
        for (size_t i = 0; i < rhs.len; ++i) {
            rhs.ptr[i] = nondet_uint8_t();
        }
    } else {
        rhs.ptr = NULL;
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&lhs));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&rhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&rhs));

    /* snapshot memory */
    uint8_t *lhs_snapshot = NULL;
    uint8_t *rhs_snapshot = NULL;
    if (lhs.len > 0) {
        lhs_snapshot = (uint8_t *)aws_mem_acquire(allocator, lhs.len);
        __CPROVER_assume(lhs_snapshot != NULL);
        memcpy(lhs_snapshot, lhs.ptr, lhs.len);
    }
    if (rhs.len > 0) {
        rhs_snapshot = (uint8_t *)aws_mem_acquire(allocator, rhs.len);
        __CPROVER_assume(rhs_snapshot != NULL);
        memcpy(rhs_snapshot, rhs.ptr, rhs.len);
    }

    struct aws_byte_cursor lhs_before = lhs;
    struct aws_byte_cursor rhs_before = rhs;

    /* call the function under verification */
    bool result = aws_byte_cursor_eq(&lhs, &rhs);

    /* postcondition: return value matches specification */
    bool expected = aws_array_eq(lhs.ptr, lhs.len, rhs.ptr, rhs.len);
    assert(result == expected);

    /* frame condition: input memory unchanged */
    if (lhs.len > 0) {
        assert(memcmp(lhs.ptr, lhs_snapshot, lhs.len) == 0);
    }
    if (rhs.len > 0) {
        assert(memcmp(rhs.ptr, rhs_snapshot, rhs.len) == 0);
    }

    /* frame condition: cursor structs unchanged */
    assert(lhs.ptr == lhs_before.ptr);
    assert(lhs.len == lhs_before.len);
    assert(rhs.ptr == rhs_before.ptr);
    assert(rhs.len == rhs_before.len);
}
