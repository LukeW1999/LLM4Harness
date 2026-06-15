#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_SIZE 256
#define MAX_BUFFER_SIZE 256

static struct aws_string *make_nondet_string(void) {
    struct aws_string *s = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
    __CPROVER_assume(s != NULL);
    s->allocator = (struct aws_allocator *)nondet_uintptr();
    s->len = nondet_size_t();
    __CPROVER_assume(s->len <= MAX_STRING_SIZE);
    /* initialise the bytes (including null‑terminator) */
    for (size_t i = 0; i < s->len + 1; ++i) {
        s->bytes[i] = nondet_char();
    }
    __CPROVER_assume(aws_string_is_valid(s));
    return s;
}

static struct aws_byte_buf *make_nondet_byte_buf(void) {
    struct aws_byte_buf *b = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(b != NULL);
    ensure_byte_buf_has_allocated_buffer_member(b);
    __CPROVER_assume(aws_byte_buf_is_bounded(b, MAX_BUFFER_SIZE));
    b->len = nondet_size_t();
    __CPROVER_assume(b->len <= b->capacity);
    /* initialise the buffer contents */
    for (size_t i = 0; i < b->len; ++i) {
        b->buffer[i] = nondet_char();
    }
    __CPROVER_assume(aws_byte_buf_is_valid(b));
    return b;
}

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = nondet_bool() ? make_nondet_string() : NULL;
    struct aws_byte_buf *buf = nondet_bool() ? make_nondet_byte_buf() : NULL;

    /* Save old immutable fields */
    struct aws_allocator *old_str_alloc = NULL;
    size_t old_str_len = 0;
    const char *old_str_bytes = NULL;
    if (str) {
        old_str_alloc = str->allocator;
        old_str_len   = str->len;
        old_str_bytes = str->bytes;
    }

    struct aws_allocator *old_buf_alloc = NULL;
    size_t old_buf_capacity = 0;
    size_t old_buf_len = 0;
    uint8_t *old_buf_buffer = NULL;
    if (buf) {
        old_buf_alloc    = buf->allocator;
        old_buf_capacity = buf->capacity;
        old_buf_len      = buf->len;
        old_buf_buffer   = buf->buffer;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Post‑conditions */
    if (!str && !buf) {
        assert(result);
    } else if (!str || !buf) {
        assert(!result);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* Immutability checks for str */
    if (str) {
        assert(str->allocator == old_str_alloc);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
    }

    /* Immutability checks for buf */
    if (buf) {
        assert(buf->allocator == old_buf_alloc);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->len == old_buf_len);
        assert(buf->buffer == old_buf_buffer);
    }

    /* Validity invariants must still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
