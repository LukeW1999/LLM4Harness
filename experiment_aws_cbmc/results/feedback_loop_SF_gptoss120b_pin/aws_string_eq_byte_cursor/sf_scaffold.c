#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Symbolic aws_string */
    struct aws_string *str = malloc(sizeof(struct aws_string));
    __CPROVER_assume(str != NULL);

    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= 1024);
    size_t total_size = sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0);
    str = realloc(str, total_size);
    __CPROVER_assume(str != NULL);

    str->allocator = allocator;
    str->len = str_len;
    for (size_t i = 0; i < str_len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* Symbolic aws_byte_cursor */
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= 1024);
    if (cur.len > 0) {
        cur.ptr = malloc(cur.len);
        __CPROVER_assume(cur.ptr != NULL);
        for (size_t i = 0; i < cur.len; ++i) {
            cur.ptr[i] = nondet_uint8_t();
        }
    } else {
        cur.ptr = NULL;
    }

    /* Preconditions */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Pre‑call snapshot */
    size_t old_str_len = str->len;
    const uint8_t *old_str_bytes = str->bytes;
    size_t old_cur_len = cur.len;
    const uint8_t *old_cur_ptr = cur.ptr;

    /* Call under verification */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* ASSERT_POSTCONDITIONS_HERE */
}
