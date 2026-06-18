#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Symbolic aws_string allocated on stack */
    const size_t MAX_LEN = 1024;
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_LEN];
    } str_obj;
    struct aws_string *str = &str_obj.s;

    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_LEN);
    str->allocator = allocator;
    str->len = str_len;
    for (size_t i = 0; i < str_len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* Symbolic aws_byte_cursor */
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

    /* Preconditions */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Pre‑call snapshot */
    size_t old_str_len = str->len;
    const uint8_t *old_str_bytes = str->bytes;
    size_t old_cur_len = cur.len;
    const uint8_t *old_cur_ptr = cur.ptr;

    /* Call under verification */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* Validity of inputs after the call */
    assert(str != NULL);
    assert(str->allocator == allocator);
    assert(aws_string_is_valid(str));

    /* Frame conditions for aws_string */
    assert(str->len == old_str_len);
    assert(str->bytes == old_str_bytes);
    (void)old_str_bytes; /* silence unused variable warning */

    /* Frame conditions for aws_byte_cursor */
    assert(cur.len == old_cur_len);
    assert(cur.ptr == old_cur_ptr);
    (void)old_cur_ptr;   /* silence unused variable warning */

    /* Result correctness */
    assert(result == (old_str_len == old_cur_len &&
                      (old_str_len == 0 ||
                       memcmp(old_str_bytes, old_cur_ptr, old_str_len) == 0)));
}
