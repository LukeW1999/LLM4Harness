#include <proof_helpers/make_common_data_structures.h>

extern bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

struct aws_string_stack_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static struct aws_string *initialize_string(struct aws_string_stack_storage *storage) {
    struct aws_string *str = (struct aws_string *)storage;

    str->allocator = aws_default_allocator();
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < str->len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[str->len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    return str;
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
    struct aws_string_stack_storage string_storage;
    struct aws_byte_cursor cursor_storage;
    uint8_t cursor_buffer[MAX_BUFFER_SIZE + 1];

    const struct aws_string *str = NULL;
    const struct aws_byte_cursor *cur = NULL;

    if (nondet_bool()) {
        str = initialize_string(&string_storage);
    }

    if (nondet_bool()) {
        initialize_byte_cursor(&cursor_storage, cursor_buffer);
        cur = &cursor_storage;
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(cur == NULL || aws_byte_cursor_is_valid(cur));

    const struct aws_string *old_str = str;
    const struct aws_byte_cursor *old_cur_ptr = cur;

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    uint8_t old_str_byte_array[MAX_BUFFER_SIZE + 1];

    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;

        for (size_t i = 0; i <= old_str_len; ++i) {
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
        __CPROVER_assert(result == true, "both NULL inputs compare equal");
    } else if (str == NULL || cur == NULL) {
        __CPROVER_assert(result == false, "exactly one NULL input does not compare equal");
    } else {
        __CPROVER_assert(
            result == byte_arrays_are_equal(str->bytes, str->len, cur->ptr, cur->len),
            "result matches byte-wise equality");
    }

    __CPROVER_assert(str == old_str, "string pointer unchanged");

    if (str != NULL) {
        __CPROVER_assert(str->allocator == old_str_allocator, "string allocator unchanged");
        __CPROVER_assert(str->len == old_str_len, "string length unchanged");

        for (size_t i = 0; i <= old_str_len; ++i) {
            __CPROVER_assert(str->bytes[i] == old_str_byte_array[i], "string byte unchanged");
        }
    }

    __CPROVER_assert(cur == old_cur_ptr, "cursor pointer unchanged");

    if (cur != NULL) {
        __CPROVER_assert(cur->len == old_cur.len, "cursor length unchanged");
        __CPROVER_assert(cur->ptr == old_cur.ptr, "cursor ptr unchanged");

        for (size_t i = 0; i < old_cur.len; ++i) {
            __CPROVER_assert(cur->ptr[i] == old_cur_byte_array[i], "cursor byte unchanged");
        }
    }
}
