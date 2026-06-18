#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256

/* nondet helpers */
size_t nondet_size_t(void);
char nondet_char(void);

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_byte_buf buf;
    char *c_str;

    /* --- allocate and initialize aws_byte_buf --- */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);
    buf.buffer = (uint8_t *)malloc(buf_capacity);
    __CPROVER_assume(buf.buffer != NULL || buf_capacity == 0);
    buf.capacity = buf_capacity;

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    /* ensure buffer pointer is valid when length is non‑zero */
    __CPROVER_assume(buf.buffer != NULL || buf.len == 0);

    buf.allocator = aws_default_allocator();

    /* --- allocate and initialize C string --- */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    c_str = (char *)malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = nondet_char();
    }
    c_str[c_len] = '\0';

    /* --- pre‑condition assumptions --- */
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* --- save copies for frame condition checks --- */
    uint8_t *old_buffer = NULL;
    if (buf.buffer != NULL && buf.len > 0) {
        old_buffer = (uint8_t *)malloc(buf.len);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, buf.buffer, buf.len);
    }
    char *old_c_str = (char *)malloc(c_len + 1);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, c_len + 1);

    /* --- call function under test --- */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* --- compute expected result manually --- */
    size_t actual_c_len = strlen(c_str);
    bool expected = false;
    if (buf.len == actual_c_len) {
        if (buf.len == 0) {
            expected = true;
        } else {
            expected = (memcmp(buf.buffer, c_str, buf.len) == 0);
        }
    }

    /* --- post‑condition assertions --- */
    assert(result == expected);                     /* return value correctness */

    /* buffer structural invariants unchanged */
    assert(buf.capacity == buf_capacity);
    assert(buf.len <= buf.capacity);
    assert(buf.allocator == aws_default_allocator());

    /* frame condition: buffer contents unchanged */
    if (buf.buffer != NULL && buf.len > 0) {
        assert(memcmp(buf.buffer, old_buffer, buf.len) == 0);
    }

    /* frame condition: C string unchanged */
    assert(strcmp(c_str, old_c_str) == 0);
}
