#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare nondeterministic inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* Allocate storage for possible non‑NULL string */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_BUFFER_SIZE + 1];
    } str_wrapper;

    /* Allocate storage for possible non‑NULL cursor */
    struct aws_byte_cursor cur_wrapper;
    uint8_t buf[MAX_BUFFER_SIZE];

    /* nondeterministically decide whether each pointer is NULL */
    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        str = &str_wrapper.s;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        str->bytes = str_wrapper.bytes;

        for (size_t i = 0; i < len; ++i) {
            str_wrapper.bytes[i] = __CPROVER_nondet_uchar();
        }
        str_wrapper.bytes[len] = 0; /* null‑terminator (not counted in len) */

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        for (size_t i = 0; i < len; ++i) {
            buf[i] = __CPROVER_nondet_uchar();
        }

        cur = &cur_wrapper;
        cur->ptr = buf;
        cur->len = len;

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    } else {
        cur = NULL;
    }

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer old_str_bytes;
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur != NULL) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Post‑condition on return value */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = true;
        if (str->len != cur->len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != cur->ptr[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* 6. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
