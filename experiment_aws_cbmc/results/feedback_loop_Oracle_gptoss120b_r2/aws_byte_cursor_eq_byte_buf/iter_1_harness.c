#include <aws/common/byte_buf.h>
#include <aws/common/byte_order.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t *nondet_uint8_t_ptr(void);

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_byte_cursor cur;
    struct aws_byte_buf    buf;

    /* ----- nondet allocation for cursor ----- */
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
    cur.ptr = (uint8_t *)malloc(cur.len);
    /* optional nondet content */
    if (cur.ptr) {
        size_t i;
        for (i = 0; i < cur.len; ++i) {
            cur.ptr[i] = nondet_uint8_t_ptr()[0];
        }
    }

    /* ----- nondet allocation for buffer ----- */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity >= buf.len);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    buf.buffer = (uint8_t *)malloc(buf.capacity);
    buf.allocator = aws_default_allocator();
    if (buf.buffer) {
        size_t i;
        for (i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t_ptr()[0];
        }
    }

    /* ----- structural validity assumptions ----- */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* ----- snapshot pre‑state ----- */
    size_t   cur_len_old   = cur.len;
    uint8_t *cur_ptr_old   = cur.ptr;
    uint8_t *cur_mem_old   = NULL;
    if (cur.ptr) {
        cur_mem_old = malloc(cur.len);
        memcpy(cur_mem_old, cur.ptr, cur.len);
    }

    size_t   buf_len_old      = buf.len;
    size_t   buf_cap_old      = buf.capacity;
    uint8_t *buf_buf_old      = buf.buffer;
    uint8_t *buf_mem_old      = NULL;
    if (buf.buffer) {
        buf_mem_old = malloc(buf.capacity);
        memcpy(buf_mem_old, buf.buffer, buf.capacity);
    }
    struct aws_allocator *buf_alloc_old = buf.allocator;

    /* ----- call under verification ----- */
    bool result = aws_byte_cursor_eq_byte_buf(&cur, &buf);

    /* ----- post‑conditions ----- */

    /* 1. return value matches the specification */
    assert(result == aws_array_eq(cur.ptr, cur.len, buf.buffer, buf.len));

    /* 2. inputs unchanged (frame condition) */
    assert(cur.len == cur_len_old);
    assert(cur.ptr == cur_ptr_old);
    if (cur.ptr) {
        assert(0 == memcmp(cur.ptr, cur_mem_old, cur.len));
    }

    assert(buf.len == buf_len_old);
    assert(buf.capacity == buf_cap_old);
    assert(buf.buffer == buf_buf_old);
    assert(buf.allocator == buf_alloc_old);
    if (buf.buffer) {
        assert(0 == memcmp(buf.buffer, buf_mem_old, buf.capacity));
    }

    /* 3. validity predicates still hold */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(aws_byte_buf_is_valid(&buf));

    return 0;
}
