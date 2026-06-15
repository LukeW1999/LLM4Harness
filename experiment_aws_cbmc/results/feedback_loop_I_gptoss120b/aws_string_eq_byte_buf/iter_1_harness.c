#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* nondeterministically decide whether to allocate a string */
    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate struct with flexible array member */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);

        /* initialize fields */
        str->allocator = aws_default_allocator();   /* allocator may be NULL for static strings */
        str->len = str_len;

        /* nondet fill the bytes */
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide whether to allocate a byte buffer */
    if (nondet_bool()) {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);

        /* allocate internal buffer and set capacity */
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));

        /* nondet set length within capacity */
        buf->len = nondet_size_t();
        __CPROVER_assume(buf->len <= buf->capacity);

        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer str_store;
    if (str != NULL) {
        old_str = *str;                     /* shallow copy of struct fields */
        save_byte_from_array(str->bytes, str->len, &str_store);
    }

    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer buf_store;
    if (buf != NULL) {
        old_buf = *buf;                     /* shallow copy of struct fields */
        save_byte_from_array(buf->buffer, buf->capacity, &buf_store);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Postconditions for return value */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* both non‑NULL */
        if (result) {
            /* equality implies same length and identical bytes */
            assert(str->len == buf->len);
            for (size_t i = 0; i < str->len; ++i) {
                assert(str->bytes[i] == buf->buffer[i]);
            }
        } else {
            /* inequality means either length differs or some byte differs */
            bool equal = true;
            if (str->len != buf->len) {
                equal = false;
            } else {
                for (size_t i = 0; i < str->len; ++i) {
                    if (str->bytes[i] != buf->buffer[i]) {
                        equal = false;
                        break;
                    }
                }
            }
            assert(!equal);
        }
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &str_store);
    }
    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert_byte_from_buffer_matches(buf->buffer, &buf_store);
    }

    /* 6. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
