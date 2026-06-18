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
        str->bytes = str_storage.data;
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
        size_t snap_size = sizeof(struct aws_string) + str->len;
        memcpy(&old_str_storage, &str_storage, snap_size);
    }

    struct aws_byte_buf old_buf = {0};
    uint8_t old_buf_mem[MAX_STRING_LEN];
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        if (buf_ptr->buffer != NULL && buf_ptr->len > 0) {
            memcpy(old_buf_mem, buf_ptr->buffer, buf_ptr->len);
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
        expected = (str->len == buf_ptr->len) &&
                   (memcmp(str->bytes, buf_ptr->buffer, str->len) == 0);
    }
    assert(result == expected);

    if (str != NULL) {
        size_t snap_size = sizeof(struct aws_string) + str->len;
        assert(memcmp(&str_storage, &old_str_storage, snap_size) == 0);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (buf_ptr->len > 0) {
            assert(memcmp(buf_ptr->buffer, old_buf.buffer, buf_ptr->len) == 0);
        }
    }
}
