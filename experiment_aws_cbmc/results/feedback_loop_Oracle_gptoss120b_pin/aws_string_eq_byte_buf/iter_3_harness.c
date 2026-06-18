#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    if (!__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        size_t alloc_size = sizeof(struct aws_string) + len - 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uchar();
        }
    }

    struct aws_byte_buf *buf_ptr = NULL;
    struct aws_byte_buf buf;
    if (!__CPROVER_nondet_bool()) {
        size_t capacity = __CPROVER_nondet_size_t();
        __CPROVER_assume(capacity <= MAX_STRING_LEN);
        uint8_t *buffer_mem = malloc(capacity);
        __CPROVER_assume(buffer_mem != NULL);
        buf.buffer = buffer_mem;
        buf.capacity = capacity;
        buf.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = __CPROVER_nondet_uchar();
        }
        buf_ptr = &buf;
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(buf_ptr == NULL || buf_ptr->capacity <= MAX_STRING_LEN);

    struct aws_string *old_str = NULL;
    if (str != NULL) {
        size_t snap_size = sizeof(struct aws_string) + str->len - 1;
        old_str = malloc(snap_size);
        __CPROVER_assume(old_str != NULL);
        memcpy(old_str, str, snap_size);
    }

    struct aws_byte_buf old_buf = {0};
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        if (buf_ptr->buffer != NULL && buf_ptr->len > 0) {
            uint8_t *old_buf_mem = malloc(buf_ptr->len);
            __CPROVER_assume(old_buf_mem != NULL);
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
        size_t snap_size = sizeof(struct aws_string) + str->len - 1;
        assert(memcmp(str, old_str, snap_size) == 0);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (buf_ptr->len > 0) {
            assert(memcmp(buf_ptr->buffer, old_buf.buffer, buf_ptr->len) == 0);
        }
    }
}
