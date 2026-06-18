#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Bounding constants are provided by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and nondeterministically allocate the string */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        /* allocate space for the struct plus a flexible array of bytes */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        /* allocate enough memory for the header and the bytes */
        str = malloc(sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0));
        if (str) {
            /* allocator may be NULL (static string) or nondet */
            str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(1);
            str->len = str_len;
            /* bytes are already part of the allocation; fill with nondet data */
            for (size_t i = 0; i < str_len; ++i) {
                ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
            }
            __CPROVER_assume(aws_string_is_valid(str));
        }
    }

    /* 2. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    /* nondet length and capacity within bounds */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity >= buf.len);
    /* fill buffer with nondet data */
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* 3. Nondeterministically decide whether to pass a NULL buffer pointer */
    struct aws_byte_buf *buf_ptr = NULL;
    if (nondet_bool()) {
        buf_ptr = &buf;
    } else {
        buf_ptr = NULL;
    }

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
}
