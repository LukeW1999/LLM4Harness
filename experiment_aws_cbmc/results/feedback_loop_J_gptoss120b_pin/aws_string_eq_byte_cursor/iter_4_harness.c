#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= max_len);

        /* allocate a wrapper on the stack that contains the flexible array */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } str_wrapper;

        str = &str_wrapper.s;

        /* set allocator (may be NULL or default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = str_len;
        /* bytes are nondeterministic; no need to initialise them */

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        struct aws_byte_cursor cur_obj;
        cur = &cur_obj;

        /* ensure the cursor has an allocated buffer member */
        ensure_byte_cursor_has_allocated_buffer_member(cur);

        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
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
        assert(result == aws_array_eq(str->bytes, str->len, cur->ptr, cur->len));
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
