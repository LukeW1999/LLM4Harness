#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    /* ---------- nondet inputs ---------- */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* ----- string ----- */
    size_t old_str_len = 0;
    struct aws_allocator *old_str_allocator = NULL;
    uint8_t *old_str_bytes = NULL;

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
            str->bytes[i] = nondet_uint8_t();
        }

        /* save old state */
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        if (str_len > 0) {
            old_str_bytes = (uint8_t *)malloc(str_len);
            __CPROVER_assume(old_str_bytes != NULL);
            for (size_t i = 0; i < str_len; ++i) {
                old_str_bytes[i] = str->bytes[i];
            }
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- cursor ----- */
    struct aws_byte_cursor old_cur;
    uint8_t *old_cur_bytes = NULL;

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

        /* save old state */
        old_cur = *cur;
        if (cur_len > 0) {
            old_cur_bytes = (uint8_t *)malloc(cur_len);
            __CPROVER_assume(old_cur_bytes != NULL);
            for (size_t i = 0; i < cur_len; ++i) {
                old_cur_bytes[i] = ((uint8_t *)cur->ptr)[i];
            }
        }

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
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
                if (str->bytes[i] != ((uint8_t *)cur->ptr)[i]) {
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
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        /* unchanged bytes */
        if (str->len > 0) {
            for (size_t i = 0; i < str->len; ++i) {
                assert(str->bytes[i] == old_str_bytes[i]);
            }
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
            for (size_t i = 0; i < cur->len; ++i) {
                assert(((uint8_t *)cur->ptr)[i] == old_cur_bytes[i]);
            }
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
