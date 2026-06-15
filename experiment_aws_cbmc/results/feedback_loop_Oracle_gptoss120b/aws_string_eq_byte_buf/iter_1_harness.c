#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

/* helper macro for implication */
#define IMPLIES(a, b) (!(a) || (b))

void aws_string_eq_byte_buf_harness(void) {
    /* allocator – not used directly in this harness */
    struct aws_allocator *allocator = aws_default_allocator();

    /* ---------- nondeterministic inputs ---------- */
    struct aws_string *str;
    if (__CPROVER_nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        /* static string – allocator set to NULL */
        str->allocator = NULL;
        str->len = len;
        /* nondet contents */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = __CPROVER_nondet_uint8();
        }
        /* null‑terminator (not counted in len) */
        str->bytes[len] = 0;
    } else {
        str = NULL;
    }

    struct aws_byte_buf buf;
    buf.capacity = MAX_STRING_LEN;
    buf.len = __CPROVER_nondet_uint();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.buffer = malloc(buf.capacity);
    __CPROVER_assume(buf.buffer != NULL);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = __CPROVER_nondet_uint8();
    }

    /* ---------- structural assumptions ---------- */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* ---------- snapshot for frame condition ---------- */
    struct aws_string *str_old = NULL;
    if (str != NULL) {
        size_t old_len = str->len;
        str_old = malloc(sizeof(struct aws_string) + old_len);
        __CPROVER_assume(str_old != NULL);
        str_old->allocator = str->allocator;
        str_old->len = old_len;
        for (size_t i = 0; i < old_len; ++i) {
            str_old->bytes[i] = str->bytes[i];
        }
    }

    struct aws_byte_buf buf_old = buf;
    uint8_t *buf_buffer_old = malloc(buf.capacity);
    __CPROVER_assume(buf_buffer_old != NULL);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf_buffer_old[i] = buf.buffer[i];
    }

    /* ---------- call the function under test ---------- */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* ---------- post‑condition: return value correctness ---------- */
    bool expected = false;
    if (str == NULL && &buf == NULL) {
        expected = true;
    } else if (str != NULL && &buf != NULL) {
        if (str->len == buf.len) {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf.buffer[i]) {
                    expected = false;
                    break;
                }
            }
        } else {
            expected = false;
        }
    } else {
        expected = false;
    }
    __CPROVER_assert(result == expected,
                     "aws_string_eq_byte_buf return value matches byte‑wise equality");

    /* ---------- post‑condition: frame condition (no modification) ---------- */
    if (str != NULL && str_old != NULL) {
        __CPROVER_assert(str->len == str_old->len, "string length unchanged");
        for (size_t i = 0; i < str->len; ++i) {
            __CPROVER_assert(str->bytes[i] == str_old->bytes[i],
                             "string bytes unchanged");
        }
    }
    __CPROVER_assert(buf.len == buf_old.len, "buffer length unchanged");
    __CPROVER_assert(buf.capacity == buf_old.capacity, "buffer capacity unchanged");
    for (size_t i = 0; i < buf.capacity; ++i) {
        __CPROVER_assert(buf.buffer[i] == buf_buffer_old[i],
                         "buffer contents unchanged");
    }

    return 0;
}
