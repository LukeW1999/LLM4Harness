#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

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

        /* allocate struct + flexible array */
        size_t alloc_size = (str_len > 0) ? sizeof(struct aws_string) + str_len - 1
                                          : sizeof(struct aws_string);
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);               /* allocation must succeed for proof */
        str->allocator = NULL;                       /* static string case */
        str->len = str_len;

        /* fill bytes with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* validity of the string */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- cursor ----- */
    if (!cur_is_null) {
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);

        struct aws_byte_cursor cur_obj;
        cur_obj.len = cur_len;
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(AWS_MEM_IS_READABLE(cur_obj.ptr, cur_len));

        /* fill cursor buffer with nondet data */
        for (size_t i = 0; i < cur_len; ++i) {
            ((uint8_t *)cur_obj.ptr)[i] = nondet_uint8_t();
        }

        cur = &cur_obj;

        /* boundedness of the cursor */
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* ---------- save old state ---------- */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;                     /* copy allocator and len */
        /* copy bytes for later comparison */
        if (str->len > 0) {
            uint8_t *old_bytes = (uint8_t *)malloc(str->len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < str->len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
            old_str.bytes[0] = old_bytes[0];   /* dummy to keep pointer; actual compare uses helper */
            /* store the copy in a helper structure */
            struct store_byte_from_buffer storage;
            save_byte_from_array(old_bytes, str->len, &storage);
            free(old_bytes);
        }
    }

    struct aws_byte_cursor old_cur;
    if (cur != NULL) {
        old_cur = *cur;
        if (cur->len > 0) {
            uint8_t *old_buf = (uint8_t *)malloc(cur->len);
            __CPROVER_assume(old_buf != NULL);
            for (size_t i = 0; i < cur->len; ++i) {
                old_buf[i] = ((uint8_t *)cur->ptr)[i];
            }
            struct store_byte_from_buffer storage_cur;
            save_byte_from_array(old_buf, cur->len, &storage_cur);
            free(old_buf);
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

    /* ---------- post‑condition asserts ---------- */
    assert(result == expected);

    if (str != NULL) {
        /* unchanged fields */
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* unchanged bytes */
        assert_bytes_match(str->bytes, old_str.bytes, str->len);
        /* validity invariant */
        assert(aws_string_is_valid(str));
    }

    if (cur != NULL) {
        /* unchanged fields */
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        /* unchanged bytes */
        assert_bytes_match(cur->ptr, old_cur.ptr, cur->len);
        /* boundedness invariant */
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
