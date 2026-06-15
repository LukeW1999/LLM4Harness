#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256
#define IMPLIES(a, b) (!(a) || (b))

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str = NULL;

    if (__CPROVER_nondet_bool()) {
        size_t len = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t str_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
        str = (struct aws_string *)str_storage;
        str->allocator = allocator;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = __CPROVER_nondet_uchar();
        }
        str->bytes[len] = 0;
    }

    struct aws_byte_buf buf;
    uint8_t buf_storage[MAX_STRING_LEN];
    buf.buffer = buf_storage;
    buf.capacity = MAX_STRING_LEN;
    buf.len = (size_t)__CPROVER_nondet_ulong();
    __CPROVER_assume(buf.len <= buf.capacity);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = __CPROVER_nondet_uchar();
    }

    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_string *str_old = NULL;
    uint8_t str_old_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    if (str != NULL) {
        str_old = (struct aws_string *)str_old_storage;
        str_old->allocator = str->allocator;
        str_old->len = str->len;
        for (size_t i = 0; i < str->len; ++i) {
            str_old->bytes[i] = str->bytes[i];
        }
    }

    struct aws_byte_buf buf_old = buf;
    uint8_t buf_buffer_old[MAX_STRING_LEN];
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf_buffer_old[i] = buf.buffer[i];
    }

    bool result = aws_string_eq_byte_buf(str, &buf);

    bool expected = false;
    if (str != NULL) {
        if (str->len == buf.len) {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf.buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    __CPROVER_assert(result == expected,
        "aws_string_eq_byte_buf return value matches byte‑wise equality");

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
}
