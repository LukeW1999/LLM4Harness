#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 64

/* nondeterministic size */
size_t nondet_size_t(void);

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_byte_cursor cur;
    struct aws_byte_buf buf;

    /* allocate memory for cursor and buffer */
    cur.ptr = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(cur.ptr != NULL);
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);

    buf.buffer = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.buffer != NULL);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);
    buf.capacity = MAX_BUFFER_SIZE;
    buf.allocator = aws_default_allocator();

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot inputs */
    struct aws_byte_cursor cur_old = cur;
    struct aws_byte_buf buf_old = buf;

    uint8_t *cur_mem_old = NULL;
    if (cur.ptr && cur.len > 0) {
        cur_mem_old = malloc(cur.len);
        __CPROVER_assume(cur_mem_old != NULL);
        memcpy(cur_mem_old, cur.ptr, cur.len);
    }

    uint8_t *buf_mem_old = NULL;
    if (buf.buffer && buf.len > 0) {
        buf_mem_old = malloc(buf.len);
        __CPROVER_assume(buf_mem_old != NULL);
        memcpy(buf_mem_old, buf.buffer, buf.len);
    }

    /* call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cur, &buf);

    /* postcondition: return value matches underlying array comparison */
    __CPROVER_assert(
        result == aws_array_eq(cur.ptr, cur.len, buf.buffer, buf.len),
        "aws_byte_cursor_eq_byte_buf return matches aws_array_eq"
    );

    /* frame conditions: inputs unchanged */
    __CPROVER_assert(cur.ptr == cur_old.ptr, "cursor ptr unchanged");
    __CPROVER_assert(cur.len == cur_old.len, "cursor len unchanged");
    __CPROVER_assert(buf.buffer == buf_old.buffer, "buffer pointer unchanged");
    __CPROVER_assert(buf.len == buf_old.len, "buffer len unchanged");
    __CPROVER_assert(buf.capacity == buf_old.capacity, "buffer capacity unchanged");
    __CPROVER_assert(buf.allocator == buf_old.allocator, "buffer allocator unchanged");

    /* memory contents unchanged */
    if (cur.ptr && cur.len > 0) {
        __CPROVER_assert(memcmp(cur.ptr, cur_mem_old, cur.len) == 0, "cursor memory unchanged");
    }
    if (buf.buffer && buf.len > 0) {
        __CPROVER_assert(memcmp(buf.buffer, buf_mem_old, buf.len) == 0, "buffer memory unchanged");
    }

    return 0;
}
