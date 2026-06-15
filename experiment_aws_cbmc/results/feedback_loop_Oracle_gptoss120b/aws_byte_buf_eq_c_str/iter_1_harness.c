#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256

/* nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
char nondet_char(void);

void aws_byte_buf_eq_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* ----- set up aws_byte_buf ----- */
    struct aws_byte_buf buf;
    buf.allocator = alloc;

    /* capacity bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;

    /* allocate buffer memory (may be NULL) */
    buf.buffer = (uint8_t *)malloc(capacity);
    if (buf.buffer) {
        /* length bounded by capacity */
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);

        /* nondet contents */
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    } else {
        /* when allocation fails, length must be zero */
        buf.len = 0;
    }

    /* ----- set up C string ----- */
    char *c_str = NULL;
    size_t c_len = nondet_size_t();
    /* allocate space for string + terminating NUL */
    c_str = (char *)malloc(c_len + 1);
    if (c_str) {
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = nondet_char();
        }
        c_str[c_len] = '\0';
    }

    /* ----- apply ground‑truth preconditions ----- */
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* ----- snapshot state for frame condition checks ----- */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    uint8_t *buf_snapshot = NULL;
    if (buf.buffer) {
        buf_snapshot = (uint8_t *)malloc(old_capacity);
        for (size_t i = 0; i < old_capacity; ++i) {
            buf_snapshot[i] = buf.buffer[i];
        }
    }

    char *c_str_snapshot = NULL;
    if (c_str) {
        size_t c_str_len = 0;
        while (c_str[c_str_len] != '\0') {
            ++c_str_len;
        }
        c_str_snapshot = (char *)malloc(c_str_len + 1);
        for (size_t i = 0; i <= c_str_len; ++i) {
            c_str_snapshot[i] = c_str[i];
        }
    }

    /* ----- call function under verification ----- */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* ----- post‑condition 1: return value correctness ----- */
    bool expected = aws_array_eq_c_str(buf.buffer, buf.len, c_str);
    assert(result == expected);

    /* ----- post‑condition 2: buffer invariants ----- */
    assert(buf.len == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* ----- post‑condition 3: frame conditions (no memory modification) ----- */
    if (buf.buffer) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == buf_snapshot[i]);
        }
    }
    if (c_str) {
        size_t i = 0;
        while (c_str_snapshot[i] != '\0') {
            assert(c_str[i] == c_str_snapshot[i]);
            ++i;
        }
        /* also check terminating NUL */
        assert(c_str[i] == '\0');
    }

    /* clean up */
    free(buf.buffer);
    free(buf_snapshot);
    free(c_str);
    free(c_str_snapshot);

    return 0;
}
