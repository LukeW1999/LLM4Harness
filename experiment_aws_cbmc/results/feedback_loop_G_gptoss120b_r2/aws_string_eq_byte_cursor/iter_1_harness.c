#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare possibly NULL inputs */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with flexible array member */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;               /* static string – allocator may be NULL */
        str->len = str_len;
        /* nondet fill the bytes */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        static struct aws_byte_cursor cur_obj;
        cur = &cur_obj;
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        cur->ptr = malloc(cur_len);
        __CPROVER_assume(cur->ptr != NULL);
        cur->len = cur_len;
        for (size_t i = 0; i < cur_len; ++i) {
            ((uint8_t *)cur->ptr)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str; /* copies allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur != NULL) {
        old_cur = *cur; /* copies ptr and len */
        save_byte_from_array((const uint8_t *)cur->ptr, cur->len, &old_cur_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Post‑condition assertions */

    /* Both NULL → true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* One NULL, the other not → false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Both non‑NULL → result matches byte‑wise equality */
    else {
        bool expected = true;
        if (str->len != cur->len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != ((const uint8_t *)cur->ptr)[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_str_bytes.buffer, str->len);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_bytes_match((const uint8_t *)cur->ptr, old_cur_bytes.buffer, cur->len);
    }

    /* 6. Validity invariants must still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
