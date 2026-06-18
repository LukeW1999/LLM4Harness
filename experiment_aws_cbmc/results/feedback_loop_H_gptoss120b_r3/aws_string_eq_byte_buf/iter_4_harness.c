#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Bounding constants are provided by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* 1. Nondeterministically create a string (or NULL) */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        uint8_t *tmp = malloc(str_len);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < str_len; ++i) {
            tmp[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(alloc, tmp, str_len);
        free(tmp);
    }

    /* 2. Create a bounded, valid byte buffer (or NULL) */
    struct aws_byte_buf buf;
    buf.allocator = alloc;
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.capacity > 0);                 /* non‑zero capacity for allocation */
    buf.buffer = malloc(buf.capacity);
    __CPROVER_assume(buf.buffer != NULL);

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    struct aws_byte_buf *buf_ptr = nondet_bool() ? &buf : NULL;

    /* 3. Save old state */
    struct aws_string old_str = {0};
    if (str) {
        old_str = *str;
    }
    struct aws_byte_buf old_buf = {0};
    if (buf_ptr) {
        old_buf = *buf_ptr;
    }

    /* 4. Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* 5. Post‑conditions */
    if (str != NULL && buf_ptr != NULL) {
        assert(result == aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len));
    } else if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* 6. Verify inputs are unchanged (except for the bytes pointer, which may be
       implementation‑defined for zero‑length strings) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
    }
    if (buf_ptr) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(buf_ptr->len == old_buf.len);
    }

    /* 7. Validity invariants must still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr) {
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    /* 8. Clean‑up */
    if (str) {
        aws_string_destroy(str);
    }
    if (buf.buffer) {
        free(buf.buffer);
    }
}
