#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_buf buf;
    const struct aws_byte_buf *buf_ptr;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate enough space for the flexible array member */
        size_t max_len = MAX_BUFFER_SIZE;
        str = malloc(sizeof(struct aws_string) + max_len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose a length within bounds */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= max_len);

        /* allocator may be NULL (static string) or a default allocator */
        str->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* the bytes are part of the allocation; no need to initialise contents */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* initialise a byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* nondeterministically set length within capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* nondeterministically decide if the buffer pointer itself is NULL */
    if (nondet_bool()) {
        buf_ptr = NULL;
    } else {
        buf_ptr = &buf;
    }

    /* 2. Save old state */
    struct aws_string *old_str = NULL;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string) + str->len);
        __CPROVER_assume(old_str != NULL);
        memcpy(old_str, str, sizeof(struct aws_string) + str->len);
    }
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* 4. Post‑condition: return value matches the specification */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
    }
    assert(result == expected);

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_bytes_match(str->bytes, old_str->bytes, str->len);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert_bytes_match(buf_ptr->buffer, old_buf.buffer, buf_ptr->len);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
    }
}
