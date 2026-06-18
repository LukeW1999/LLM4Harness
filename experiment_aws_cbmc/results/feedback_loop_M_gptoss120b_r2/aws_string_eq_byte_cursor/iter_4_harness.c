#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    /* ---------- nondet inputs ---------- */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* ----- string ----- */
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* stack-allocated buffer large enough for struct + bytes */
        uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_storage;

        str->allocator = NULL;               /* static string case */
        str->len = str_len;

        /* fill bytes with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* validity of the string */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- cursor ----- */
    struct aws_byte_cursor cur_obj;
    if (!cur_is_null) {
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);

        cur_obj.len = cur_len;

        /* stack-allocated buffer for cursor */
        uint8_t cur_buf[MAX_BUFFER_SIZE];
        cur_obj.ptr = cur_buf;

        /* fill cursor buffer with nondet data */
        for (size_t i = 0; i < cur_len; ++i) {
            cur_buf[i] = nondet_uint8_t();
        }

        cur = &cur_obj;

        /* boundedness of the cursor */
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* ---------- save old state ---------- */
    struct aws_string old_str;
    uint8_t *old_str_bytes = NULL;
    if (str != NULL) {
        old_str = *str;                     /* copy allocator and len */
        if (str->len > 0) {
            old_str_bytes = (uint8_t *)malloc(str->len);
            __CPROVER_assume(old_str_bytes != NULL);
            for (size_t i = 0; i < str->len; ++i) {
                old_str_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
            struct store_byte_from_buffer storage;
            save_byte_from_array(old_str_bytes, str->len, &storage);
        }
    }

    struct aws_byte_cursor old_cur;
    uint8_t *old_cur_bytes = NULL;
    if (cur != NULL) {
        old_cur = *cur;
        if (cur->len > 0) {
            old_cur_bytes = (uint8_t *)malloc(cur->len);
            __CPROVER_assume(old_cur_bytes != NULL);
            for (size_t i = 0; i < cur->len; ++i) {
                old_cur_bytes[i] = ((uint8_t *)cur->ptr)[i];
            }
            struct store_byte_from_buffer storage_cur;
            save_byte_from_array(old_cur_bytes, cur->len, &storage_cur);
        }
    }

    /* ---------- call function under test ---------- */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* ---------- compute expected result ---------- */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        if (str->len != cur->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (((uint8_t *)str->bytes)[i] != ((uint8_t *)cur->ptr)[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    /* ---------- post-condition asserts ---------- */
    assert(result == expected);

    if (str != NULL) {
        /* unchanged fields */
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* unchanged bytes */
        if (str->len > 0) {
            assert_bytes_match(str->bytes, old_str_bytes, str->len);
        }
        /* validity invariant */
        assert(aws_string_is_valid(str));
    }

    if (cur != NULL) {
        /* unchanged fields */
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        /* unchanged bytes */
        if (cur->len > 0) {
            assert_bytes_match(cur->ptr, old_cur_bytes, cur->len);
        }
        /* boundedness invariant */
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* free auxiliary copies */
    if (old_str_bytes != NULL) {
        free(old_str_bytes);
    }
    if (old_cur_bytes != NULL) {
        free(old_cur_bytes);
    }
}
