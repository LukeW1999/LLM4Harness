#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness(void) {
    /*--- aws_string *str ---------------------------------------------------*/
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        unsigned char buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;
        str->allocator = aws_default_allocator();
        str->len = len;
        /* bytes are the flexible array member that follows the struct */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /*--- aws_byte_cursor *cur ---------------------------------------------*/
    struct aws_byte_cursor cur;
    struct aws_byte_cursor *cur_ptr;
    if (nondet_bool()) {
        cur_ptr = NULL;
    } else {
        cur_ptr = &cur;
        ensure_byte_cursor_has_allocated_buffer_member(&cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    }

    /*--- Save old state ----------------------------------------------------*/
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur_ptr != NULL) {
        save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);
    }

    /*--- Call function -----------------------------------------------------*/
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /*--- Post‑condition checks ---------------------------------------------*/
    /* Both NULL */
    assert(!(str == NULL && cur_ptr == NULL) || result == true);
    /* One NULL */
    assert(!((str == NULL) ^ (cur_ptr == NULL)) || result == false);
    /* Both non‑NULL */
    if (str != NULL && cur_ptr != NULL) {
        assert(result == aws_array_eq(str->bytes, str->len, cur.ptr, cur.len));
    }

    /*--- Memory safety checks ---------------------------------------------*/
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur_ptr != NULL) {
        assert(cur.ptr == old_cur.ptr);
        assert(cur.len == old_cur.len);
        assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);
    }

    /*--- Structural validity ------------------------------------------------*/
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
