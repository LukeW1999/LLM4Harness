#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256
#define IMPLIES(a, b) (!(a) || (b))

void aws_string_eq_byte_buf_harness(void) {
    /* nondeterministic inputs */
    struct aws_string *str;
    struct aws_byte_buf buf;

    /* allocate and initialize buf */
    buf.buffer = malloc(MAX_STRING_LEN);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = MAX_STRING_LEN;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    /* fill buffer with nondet bytes */
    for (size_t i = 0; i < buf.capacity; ++i) {
        ((uint8_t *)buf.buffer)[i] = nondet_uint8_t();
    }

    /* allocate and initialize str (may be NULL) */
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;               /* static string for proof */
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot old state for frame condition checks */
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = malloc(buf.capacity);
    __CPROVER_assume(old_buf_buffer != NULL);
    memcpy(old_buf_buffer, buf.buffer, buf.capacity);

    size_t old_str_len = 0;
    uint8_t *old_str_bytes = NULL;
    if (str != NULL) {
        old_str_len = str->len;
        old_str_bytes = malloc(old_str_len);
        __CPROVER_assume(old_str_bytes != NULL);
        memcpy(old_str_bytes, str->bytes, old_str_len);
    }

    /* call the function under verification */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* postconditions */

    /* 1. Return value correctness */
    if (str == NULL) {
        /* buf is never NULL (address taken), so result must be false */
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf.buffer, buf.len);
        assert(result == expected);
    }

    /* 2. Buffer invariants (no modification) */
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(memcmp(buf.buffer, old_buf_buffer, buf.capacity) == 0);

    /* 3. String invariants (no modification) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(memcmp(str->bytes, old_str_bytes, old_str_len) == 0);
    }

    /* clean up */
    free(old_buf_buffer);
    free(buf.buffer);
    if (old_str_bytes) free(old_str_bytes);
    if (str) free((void *)str);

    return 0;
}
