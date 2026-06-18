#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256
#define MAX_CSTR_LEN    256

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
char nondet_char(void);

void aws_byte_buf_eq_c_str_harness(void) {
    /* allocator (not used by the function but part of the struct) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* --- set up aws_byte_buf ------------------------------------------------*/
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.buffer = malloc(capacity);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* fill buffer with nondet data */
    for (size_t i = 0; i < buf.capacity; ++i) {
        ((uint8_t *)buf.buffer)[i] = nondet_uint8_t();
    }

    /* --- set up null‑terminated C string ------------------------------------*/
    char *c_str = malloc(MAX_CSTR_LEN);
    __CPROVER_assume(c_str != NULL);
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_CSTR_LEN);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = nondet_char();
    }
    c_str[c_len] = '\0';

    /* --- apply the required preconditions -----------------------------------*/
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* --- save old state for frame condition checks --------------------------*/
    size_t old_len      = buf.len;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    uint8_t *old_buf_copy = malloc(old_capacity);
    __CPROVER_assume(old_buf_copy != NULL);
    memcpy(old_buf_copy, buf.buffer, old_capacity);

    size_t old_cstr_len = strlen(c_str);
    char *old_cstr_copy = malloc(old_cstr_len + 1);
    __CPROVER_assume(old_cstr_copy != NULL);
    memcpy(old_cstr_copy, c_str, old_cstr_len + 1);

    /* --- call the function under verification --------------------------------*/
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* --- post‑condition 1: return value correctness ------------------------*/
    bool expected;
    if (buf.len == old_cstr_len) {
        expected = (memcmp(buf.buffer, c_str, buf.len) == 0);
    } else {
        expected = false;
    }
    __CPROVER_assert(result == expected,
                     "aws_byte_buf_eq_c_str return value matches expected equality");

    /* --- post‑condition 2: buffer invariants --------------------------------*/
    __CPROVER_assert(buf.len == old_len,
                     "aws_byte_buf_eq_c_str does not modify buf.len");
    __CPROVER_assert(buf.capacity == old_capacity,
                     "aws_byte_buf_eq_c_str does not modify buf.capacity");
    __CPROVER_assert(buf.allocator == old_allocator,
                     "aws_byte_buf_eq_c_str does not modify buf.allocator");

    /* --- post‑condition 3: frame conditions (no memory modification) --------*/
    __CPROVER_assert(memcmp(buf.buffer, old_buf_copy, old_capacity) == 0,
                     "aws_byte_buf_eq_c_str does not modify buf.buffer contents");
    __CPROVER_assert(strcmp(c_str, old_cstr_copy) == 0,
                     "aws_byte_buf_eq_c_str does not modify the C string");

    return 0;
}
