#include <proof_helpers/make_common_data_structures.h>

/* Harness for aws_string_eq_byte_buf */
void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and bound data structures */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Allocate and nondet‑fill the raw bytes for the string */
    uint8_t *raw_bytes = malloc(str_len);
    __CPROVER_assume(raw_bytes != NULL);
    for (size_t i = 0; i < str_len; ++i) {
        raw_bytes[i] = nondet_uint8_t();
    }

    /* Create a valid aws_string from the raw bytes */
    struct aws_string *str = aws_string_new_from_array(aws_default_allocator(), raw_bytes, str_len);
    __CPROVER_assume(str != NULL);
    /* aws_string_new_from_array adds a null‑terminator internally */

    /* Prepare a byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.len <= buf.capacity);
    for (size_t i = 0; i < buf.len; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* 2. Save old state BEFORE calling */
    struct aws_string old_str = *str;
    uint8_t *old_str_bytes = NULL;
    if (str_len > 0) {
        old_str_bytes = malloc(str_len);
        __CPROVER_assume(old_str_bytes != NULL);
        memcpy(old_str_bytes, str->bytes, str_len);
    }

    struct aws_byte_buf old_buf = buf;
    uint8_t *old_buf_bytes = NULL;
    if (buf.len > 0) {
        old_buf_bytes = malloc(buf.len);
        __CPROVER_assume(old_buf_bytes != NULL);
        memcpy(old_buf_bytes, buf.buffer, buf.len);
    }

    /* 3. Call function under test */
    (void)aws_string_eq_byte_buf(str, &buf);

    /* 4. Assert postconditions for both return paths */
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    if (str_len > 0) {
        for (size_t i = 0; i < str_len; ++i) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    if (buf.len > 0) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == old_buf_bytes[i]);
        }
    }

    /* 5. Validity invariants must still hold */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
