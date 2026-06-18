#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* buffers that must outlive the conditional blocks */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } str_wrapper;

    uint8_t cur_buf[MAX_BUFFER_SIZE];
    struct aws_byte_cursor cur_obj;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        str = &str_wrapper.s;

        /* allocator may be NULL or the default */
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;

        str->len = str_len;
        str->bytes = (const uint8_t *)str_wrapper.bytes; /* assign backing buffer */

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);

        cur = &cur_obj;
        cur->ptr = cur_buf;
        cur->len = cur_len;

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* Save old state for immutability checks */
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    if (str) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
    }

    const uint8_t *old_cur_ptr = NULL;
    size_t old_cur_len = 0;
    if (cur) {
        old_cur_ptr = cur->ptr;
        old_cur_len = cur->len;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Assert postconditions */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        struct aws_byte_cursor str_cur = aws_byte_cursor_from_string(str);
        assert(result == aws_byte_cursor_eq(&str_cur, cur));
    }

    /* Assert unchanged fields (immutability) */
    if (str) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
    }
    if (cur) {
        assert(cur->ptr == old_cur_ptr);
        assert(cur->len == old_cur_len);
    }

    /* Assert validity invariants still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
