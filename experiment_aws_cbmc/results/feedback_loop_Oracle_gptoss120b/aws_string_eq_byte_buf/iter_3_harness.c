#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256
#define IMPLIES(a, b) (!(a) || (b))

void aws_string_eq_byte_buf_harness(void) {
    /* allocator – not used directly in this harness */
    struct aws_allocator *allocator = aws_default_allocator();

    /* ---------- nondeterministic inputs ---------- */
    struct aws_string *str = NULL;

    if (__CPROVER_nondet_bool()) {
        /* allocate a string with a nondeterministic length on the stack */
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t str_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
        str = (struct aws_string *)str_storage;
        str->allocator = NULL;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uint8();
        }
        ((uint8_t *)str->bytes)[len] = 0; /* null‑terminator (not counted in len) */
    }

    struct aws_byte_buf buf;
    uint8_t buf_storage[MAX_STRING_LEN];
    buf.buffer = buf_storage;
    buf.capacity = MAX_STRING_LEN;
    buf.len = __CPROVER_nondet_uint();
    __CPROVER_assume(buf.len <= buf.capacity);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = __CPROVER_nondet_uint8();
    }

    /* ---------- structural assumptions ---------- */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* ---------- snapshot for frame condition ---------- */
    struct aws_string *str_old = NULL;
    uint8_t str_old_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    if (str != NULL) {
        str_old = (struct aws_string *)str_old_storage;
        str_old->allocator = str->allocator;
        str_old->len = str->len;
        for (size_t i = 0; i < str->len; ++i) {
            ((uint8_t *)str_old->bytes)[i] = ((uint8_t *)str->bytes)[i];
        }
    }

    struct aws_byte_buf buf_old = buf;
    uint8_t buf_buffer_old[MAX_STRING_LEN];
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf_buffer_old[i] = buf.buffer[i];
    }

    /* ---------- call the function under test ---------- */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* ---------- post‑condition: return value correctness ---------- */
    bool expected = false;
    if (str != NULL && &buf != NULL) {
        if (str->len == buf.len) {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (((uint8_t *)str->bytes)[i] != buf.buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    __CPROVER_assert(result == expected,
                     "aws_string_eq_byte_buf return value matches byte‑wise equality");

    /* ---------- post‑condition: frame condition (no modification) ---------- */
    if (str != NULL && str_old != NULL) {
        __CPROVER_assert(str->len == str_old->len, "string length unchanged");
        for (size_t i = 0; i < str->len; ++i) {
            __CPROVER_assert(((uint8_t *)str->bytes)[i] == ((uint8_t *)str_old->bytes)[i],
                             "string bytes unchanged");
        }
    }
    __CPROVER_assert(buf.len == buf_old.len, "buffer length unchanged");
    __CPROVER_assert(buf.capacity == buf_old.capacity, "buffer capacity unchanged");
    for (size_t i = 0; i < buf.capacity; ++i) {
        __CPROVER_assert(buf.buffer[i] == buf_buffer_old[i],
                         "buffer contents unchanged");
    }
}
