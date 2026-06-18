#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct aws_string_storage_for_harness {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static void initialize_string_storage(struct aws_string_storage_for_harness *storage) {
    struct aws_allocator *allocator = aws_default_allocator();

    storage->allocator = allocator;
    storage->len = nondet_size_t();
    __CPROVER_assume(storage->len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < storage->len; ++i) {
        storage->bytes[i] = nondet_uint8_t();
    }
    storage->bytes[storage->len] = 0;
}

static void initialize_byte_cursor(struct aws_byte_cursor *cur, uint8_t *buffer) {
    cur->len = nondet_size_t();
    __CPROVER_assume(cur->len <= MAX_BUFFER_SIZE);

    if (cur->len == 0 && nondet_bool()) {
        cur->ptr = NULL;
    } else {
        cur->ptr = buffer;
    }

    for (size_t i = 0; i < cur->len; ++i) {
        buffer[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(cur));
}

static bool byte_arrays_are_equal(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len) {
    if (a_len != b_len) {
        return false;
    }

    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string_storage_for_harness string_storage;
    struct aws_byte_cursor cursor_storage;
    uint8_t cursor_buffer[MAX_BUFFER_SIZE + 1];

    const struct aws_string *str = NULL;
    const struct aws_byte_cursor *cur = NULL;

    if (nondet_bool()) {
        initialize_string_storage(&string_storage);
        str = (const struct aws_string *)&string_storage;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        initialize_byte_cursor(&cursor_storage, cursor_buffer);
        cur = &cursor_storage;
    }

    const struct aws_string *old_str = str;
    const struct aws_byte_cursor *old_cur_ptr = cur;

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    const uint8_t *old_str_bytes = NULL;
    uint8_t old_str_byte_array[MAX_BUFFER_SIZE + 1];

    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        old_str_bytes = str->bytes;

        for (size_t i = 0; i < old_str_len + 1; ++i) {
            old_str_byte_array[i] = str->bytes[i];
        }
    }

    struct aws_byte_cursor old_cur;
    uint8_t old_cur_byte_array[MAX_BUFFER_SIZE + 1];

    if (cur != NULL) {
        old_cur = *cur;

        for (size_t i = 0; i < cur->len; ++i) {
            old_cur_byte_array[i] = cur->ptr[i];
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        assert(result == byte_arrays_are_equal(str->bytes, str->len, cur->ptr, cur->len));
    }

    assert(str == old_str);

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);

        for (size_t i = 0; i < old_str_len + 1; ++i) {
            assert(str->bytes[i] == old_str_byte_array[i]);
        }
    }

    assert(cur == old_cur_ptr);

    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);

        for (size_t i = 0; i < old_cur.len; ++i) {
            assert(cur->ptr[i] == old_cur_byte_array[i]);
        }
    }
}
