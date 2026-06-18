#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    struct {
        struct aws_string s;
        uint8_t data[MAX_STRING_LEN];
    } str_storage;

    if (!__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = &str_storage.s;
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str_storage.data[i] = __CPROVER_nondet_uchar();
        }
    }

    struct aws_byte_buf *buf_ptr = NULL;
    struct aws_byte_buf buf;
    uint8_t buf_mem[MAX_STRING_LEN];

    if (!__CPROVER_nondet_bool()) {
        size_t capacity = __CPROVER_nondet_size_t();
        __CPROVER_assume(capacity <= MAX_STRING_LEN);
        buf.buffer = buf_mem;
        buf.capacity = capacity;
        buf.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = __CPROVER_nondet_uchar();
        }
        buf_ptr = &buf;
    }

    __CPROVER_assume(str == NULL || str->len <= MAX_STRING_LEN);
    __CPROVER_assume(buf_ptr == NULL || buf_ptr->capacity <= MAX_STRING_LEN);

    struct {
        struct aws_string s;
        uint8_t data[MAX_STRING_LEN];
    } old_str_storage;
    if (str != NULL) {
        old_str_storage.s.allocator = str_storage.s.allocator;
        old_str_storage.s.len = str_storage.s.len;
        for (size_t i = 0; i < str->len; ++i) {
            old_str_storage.data[i] = str_storage.data[i];
        }
    }

    struct aws_byte_buf old_buf = {0};
    uint8_t old_buf_mem[MAX_STRING_LEN];
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        if (buf_ptr->buffer != NULL && buf_ptr->len > 0) {
            for (size_t i = 0; i < buf_ptr->len; ++i) {
                old_buf_mem[i] = buf_ptr->buffer[i];
            }
            old_buf.buffer = old_buf_mem;
        }
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = true;
        if (str->len != buf_ptr->len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf_ptr->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    if (str != NULL) {
        assert(old_str_storage.s.allocator == str_storage.s.allocator);
        assert(old_str_storage.s.len == str_storage.s.len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(old_str_storage.data[i] == str_storage.data[i]);
        }
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (buf_ptr->len > 0) {
            for (size_t i = 0; i < buf_ptr->len; ++i) {
                assert(buf_ptr->buffer[i] == old_buf.buffer[i]);
            }
        }
    }
}
