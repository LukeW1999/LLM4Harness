#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Bounding constants are provided by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and nondeterministically allocate the string */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        uint8_t *tmp = malloc(str_len);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < str_len; ++i) {
            tmp[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(aws_default_allocator(), tmp, str_len);
        free(tmp);
    }

    /* 2. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity >= buf.len);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* 3. Nondeterministically decide whether to pass a NULL buffer pointer */
    struct aws_byte_buf *buf_ptr = nondet_bool() ? &buf : NULL;

    /* 4. Save old state before the call */
    struct aws_string old_str = {0};
    if (str) {
        old_str = *str;
    }
    struct aws_byte_buf old_buf = {0};
    if (buf_ptr) {
        old_buf = *buf_ptr;
    }

    /* 5. Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* 6. Post‑condition: result must equal the underlying array comparison */
    if (str != NULL && buf_ptr != NULL) {
        assert(result == aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len));
    } else if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* 7. Unchanged fields (the function does not modify any input) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
    }
    if (buf_ptr) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(buf_ptr->len == old_buf.len);
    }

    /* 8. Validity invariants must still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr) {
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    if (str) {
        aws_string_destroy(str);
    }
}
