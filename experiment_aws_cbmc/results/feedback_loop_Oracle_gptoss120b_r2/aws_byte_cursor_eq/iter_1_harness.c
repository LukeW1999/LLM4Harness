#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Maximum size for the byte cursor buffers used in the proof */
#define MAX_BUFFER_SIZE 256

/* Nondeterministic helpers provided by CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

/* Frame condition helpers */
static void copy_memory(const uint8_t *src, uint8_t *dst, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

/* Harness */
void aws_byte_cursor_eq_harness(void) {
    struct aws_byte_cursor lhs, rhs;
    uint8_t *buf_lhs = malloc(MAX_BUFFER_SIZE);
    uint8_t *buf_rhs = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_lhs != NULL);
    __CPROVER_assume(buf_rhs != NULL);

    /* Initialize buffers with nondeterministic data */
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        buf_lhs[i] = nondet_uint8_t();
        buf_rhs[i] = nondet_uint8_t();
    }

    /* Nondeterministic lengths bounded by MAX_BUFFER_SIZE */
    lhs.len = nondet_size_t();
    rhs.len = nondet_size_t();
    __CPROVER_assume(lhs.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(rhs.len <= MAX_BUFFER_SIZE);

    lhs.ptr = buf_lhs;
    rhs.ptr = buf_rhs;

    /* Structural validity assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&lhs));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&rhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&rhs));

    /* Snapshot for frame condition checks */
    struct aws_byte_cursor lhs_old = lhs;
    struct aws_byte_cursor rhs_old = rhs;
    uint8_t buf_lhs_old[MAX_BUFFER_SIZE];
    uint8_t buf_rhs_old[MAX_BUFFER_SIZE];
    copy_memory(buf_lhs, buf_lhs_old, MAX_BUFFER_SIZE);
    copy_memory(buf_rhs, buf_rhs_old, MAX_BUFFER_SIZE);

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&lhs, &rhs);

    /* Postcondition 1: Return value correctness */
    if (lhs.len != rhs.len) {
        assert(result == false);
    } else {
        bool mem_eq = true;
        for (size_t i = 0; i < lhs.len; ++i) {
            if (lhs.ptr[i] != rhs.ptr[i]) {
                mem_eq = false;
                break;
            }
        }
        assert(result == mem_eq);
    }

    /* Postcondition 2: No modification of input structures */
    assert(lhs.ptr == lhs_old.ptr);
    assert(lhs.len == lhs_old.len);
    assert(rhs.ptr == rhs_old.ptr);
    assert(rhs.len == rhs_old.len);

    /* Postcondition 3: No modification of pointed-to memory */
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        assert(buf_lhs[i] == buf_lhs_old[i]);
        assert(buf_rhs[i] == buf_rhs_old[i]);
    }

    /* Clean up */
    free(buf_lhs);
    free(buf_rhs);
    return 0;
}
