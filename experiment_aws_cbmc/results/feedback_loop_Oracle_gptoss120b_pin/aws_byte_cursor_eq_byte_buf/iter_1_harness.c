#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* allocate cursor */
    struct aws_byte_cursor cur;
    cur.ptr = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(cur.ptr != NULL);
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);

    /* allocate buffer */
    struct aws_byte_buf buf;
    buf.buffer = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.buffer != NULL);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL; /* allocator not used in this function */

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save copies for frame condition checks */
    struct aws_byte_cursor cur_old = cur;
    struct aws_byte_buf   buf_old = buf;

    uint8_t *cur_mem_old = NULL;
    if (cur.len > 0) {
        cur_mem_old = malloc(cur.len);
        __CPROVER_assume(cur_mem_old != NULL);
        memcpy(cur_mem_old, cur.ptr, cur.len);
    }

    uint8_t *buf_mem_old = NULL;
    if (buf.len > 0) {
        buf_mem_old = malloc(buf.len);
        __CPROVER_assume(buf_mem_old != NULL);
        memcpy(buf_mem_old, buf.buffer, buf.len);
    }

    /* call the function under verification */
    bool result = aws_byte_cursor_eq_byte_buf(&cur, &buf);

    /* post‑condition 1: return value matches the specification */
    bool expected = aws_array_eq(cur.ptr, cur.len, buf.buffer, buf.len);
    assert(result == expected);

    /* post‑condition 2: inputs are unchanged (frame condition) */
    assert(cur.ptr == cur_old.ptr);
    assert(cur.len == cur_old.len);
    assert(buf.buffer == buf_old.buffer);
    assert(buf.len == buf_old.len);
    assert(buf.capacity == buf_old.capacity);
    assert(buf.allocator == buf_old.allocator);

    if (cur_mem_old) {
        assert(memcmp(cur.ptr, cur_mem_old, cur.len) == 0);
        free(cur_mem_old);
    }
    if (buf_mem_old) {
        assert(memcmp(buf.buffer, buf_mem_old, buf.len) == 0);
        free(buf_mem_old);
    }

    free(cur.ptr);
    free(buf.buffer);
    return 0;
}
