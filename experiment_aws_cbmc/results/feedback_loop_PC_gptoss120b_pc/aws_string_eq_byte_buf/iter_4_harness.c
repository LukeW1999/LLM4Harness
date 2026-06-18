#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate raw bytes for the string */
        uint8_t *raw = malloc(len);
        __CPROVER_assume(raw != NULL || len == 0);
        for (size_t i = 0; i < len; ++i) {
            raw[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(allocator, raw, len);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide if buf is NULL */
    if (nondet_bool()) {
        buf_ptr = NULL;
    } else {
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
        aws_byte_buf_init(&buf, allocator, cap);

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= cap);
        buf.len = len;

        for (size_t i = 0; i < len; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
        buf_ptr = &buf;
    }

    /* Save old state for immutability checks */
    struct aws_string *old_str = str;
    struct aws_string old_str_copy;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str_copy = *str;
        if (str->len > 0) {
            save_byte_from_array(str->bytes, str->len, &old_str_bytes);
        }
    }

    struct aws_byte_buf *old_buf = buf_ptr;
    struct aws_byte_buf old_buf_copy;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf_ptr != NULL) {
        old_buf_copy = *buf_ptr;
        if (buf_ptr->len > 0) {
            save_byte_from_array(buf_ptr->buffer, buf_ptr->len, &old_buf_bytes);
        }
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Compute expected result according to specification */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
    }

    /* Assert return value matches expectation */
    assert(result == expected);

    /* Assert unchanged fields (frame conditions) */
    if (str != NULL) {
        assert(str->allocator == old_str_copy.allocator);
        assert(str->len == old_str_copy.len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
        }
    } else {
        assert(old_str == NULL);
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf_copy.allocator);
        assert(buf_ptr->capacity == old_buf_copy.capacity);
        assert(buf_ptr->len == old_buf_copy.len);
        assert(buf_ptr->buffer == old_buf_copy.buffer);
        if (buf_ptr->len > 0) {
            assert_byte_from_buffer_matches(buf_ptr->buffer, &old_buf_bytes);
        }
    } else {
        assert(old_buf == NULL);
    }

    /* Assert validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
    }

    /* Clean up */
    if (str != NULL) {
        aws_string_destroy(str);
    }
    if (buf_ptr != NULL) {
        aws_byte_buf_clean_up(buf_ptr);
    }
}
