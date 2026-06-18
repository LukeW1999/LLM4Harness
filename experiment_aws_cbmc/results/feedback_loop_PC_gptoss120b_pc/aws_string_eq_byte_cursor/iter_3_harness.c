#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

/* Helper to copy a byte array */
static void copy_bytes(const uint8_t *src, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
}

void aws_string_eq_byte_cursor_harness(void) {
    /* ---------- 1. Declare and bound inputs ---------- */
    struct aws_string *str;
    struct aws_byte_cursor cur;

    /* nondet decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a buffer on the stack for the string (struct + flexible array) */
        uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_storage;
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        str->allocator = NULL;               /* static string – allocator may be NULL */
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
    }

    /* nondet decide whether cur.ptr is NULL (aws_byte_cursor permits NULL ptr with len 0) */
    if (nondet_bool()) {
        cur.ptr = NULL;
        cur.len = 0;
    } else {
        uint8_t cur_buf[MAX_BUFFER_SIZE];
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        cur.ptr = cur_buf;
        cur.len = cur_len;
        for (size_t i = 0; i < cur_len; ++i) {
            ((uint8_t *)cur.ptr)[i] = nondet_uint8_t();
        }
    }

    /* ---------- 2. Assume validity of inputs ---------- */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* ---------- 3. Save old state (frame conditions) ---------- */
    struct aws_string *old_str = str;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    uint8_t *old_str_bytes = NULL;
    if (str != NULL && old_str_len > 0) {
        old_str_bytes = malloc(old_str_len);
        __CPROVER_assume(old_str_bytes != NULL);
        copy_bytes(str->bytes, old_str_bytes, old_str_len);
    }

    struct aws_byte_cursor old_cur = cur;
    uint8_t *old_cur_bytes = NULL;
    if (cur.ptr != NULL && cur.len > 0) {
        old_cur_bytes = malloc(cur.len);
        __CPROVER_assume(old_cur_bytes != NULL);
        copy_bytes(cur.ptr, old_cur_bytes, cur.len);
    }

    /* ---------- 4. Call function under test ---------- */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* ---------- 5. Post‑condition: return value semantics ---------- */
    if (str == NULL && cur.ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur.ptr == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str->len == cur.len);
            for (size_t i = 0; i < str->len; ++i) {
                assert(str->bytes[i] == ((const uint8_t *)cur.ptr)[i]);
            }
        } else {
            bool lengths_equal = (str->len == cur.len);
            bool all_bytes_equal = true;
            if (lengths_equal) {
                for (size_t i = 0; i < str->len; ++i) {
                    if (str->bytes[i] != ((const uint8_t *)cur.ptr)[i]) {
                        all_bytes_equal = false;
                        break;
                    }
                }
            } else {
                all_bytes_equal = false;
            }
            assert(! (lengths_equal && all_bytes_equal));
        }
    }

    /* ---------- 6. Frame conditions: inputs unchanged ---------- */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        for (size_t i = 0; i < old_str_len; ++i) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
    }
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    if (cur.ptr != NULL && cur.len > 0) {
        for (size_t i = 0; i < cur.len; ++i) {
            assert(((const uint8_t *)cur.ptr)[i] == old_cur_bytes[i]);
        }
    }

    /* ---------- 7. Invariants after the call ---------- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* clean up */
    if (old_str_bytes != NULL) {
        free(old_str_bytes);
    }
    if (old_cur_bytes != NULL) {
        free(old_cur_bytes);
    }
}
