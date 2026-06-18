#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

/* Harness for aws_string_eq_byte_buf */
void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and bound data structures */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* allocate space for the string (header + flexible array + null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + str_len + 1);
    __CPROVER_assume(str != NULL);
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = str_len;

    /* nondet fill the string bytes */
    for (size_t i = 0; i < str_len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[str_len] = 0; /* null‑terminator */

    /* assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

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
