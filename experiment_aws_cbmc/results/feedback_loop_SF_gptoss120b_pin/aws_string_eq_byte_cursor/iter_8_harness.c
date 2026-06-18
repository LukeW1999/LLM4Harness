#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <string.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    const size_t MAX_LEN = 1024;

    /* Prepare nondeterministic string data */
    uint8_t str_buf[MAX_LEN];
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_LEN);
    for (size_t i = 0; i < str_len; ++i) {
        str_buf[i] = nondet_uint8_t();
    }

    struct aws_string *str = aws_string_new_from_array(allocator, str_buf, str_len);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Prepare nondeterministic byte cursor */
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_LEN);
    uint8_t cur_buf[MAX_LEN];
    if (cur.len > 0) {
        cur.ptr = cur_buf;
        for (size_t i = 0; i < cur.len; ++i) {
            cur.ptr[i] = nondet_uint8_t();
        }
    } else {
        cur.ptr = NULL;
    }

    /* Save old values for post‑condition checks */
    size_t old_str_len = str->len;
    const uint8_t *old_str_bytes = str->bytes;
    size_t old_cur_len = cur.len;
    const uint8_t *old_cur_ptr = cur.ptr;

    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* Verify that inputs are unchanged */
    assert(str != NULL);
    assert(str->allocator == allocator);
    assert(aws_string_is_valid(str));
    assert(str->len == old_str_len);
    assert(str->bytes == old_str_bytes);

    assert(cur.len == old_cur_len);
    assert(cur.ptr == old_cur_ptr);

    /* Verify the result matches the specification */
    assert(result == (old_str_len == old_cur_len &&
                      (old_str_len == 0 ||
                       memcmp(old_str_bytes, old_cur_ptr, old_str_len) == 0)));
}
