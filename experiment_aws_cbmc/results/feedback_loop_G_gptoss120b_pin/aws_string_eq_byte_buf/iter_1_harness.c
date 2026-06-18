#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* nondeterministic choice for NULL or non‑NULL string */
    const struct aws_string *str;
    if (nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_string *tmp = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(tmp != NULL);
        tmp->allocator = NULL;               /* static string – allocator may be NULL */
        tmp->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)tmp->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    } else {
        str = NULL;
    }

    /* nondeterministic choice for NULL or non‑NULL byte buffer */
    const struct aws_byte_buf *buf;
    struct aws_byte_buf tmp_buf;
    if (nondet_bool()) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&tmp_buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&tmp_buf);
        /* len must be <= capacity */
        __CPROVER_assume(tmp_buf.len <= tmp_buf.capacity);
        for (size_t i = 0; i < tmp_buf.capacity; ++i) {
            tmp_buf.buffer[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_byte_buf_is_valid(&tmp_buf));
        buf = &tmp_buf;
    } else {
        buf = NULL;
    }

    /* Save old state for immutability checks */
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_buf_bytes;
    if (buf != NULL) {
        save_byte_from_array(buf->buffer, buf->capacity, &old_buf_bytes);
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Post‑condition: return value matches specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* Both non‑NULL: result true iff lengths equal and all bytes equal */
        if (result) {
            assert(str->len == buf->len);
            assert_bytes_match(str->bytes, buf->buffer, str->len);
        } else {
            /* If result is false then either lengths differ or some byte differs */
            assert(!(str->len == buf->len &&
                     /* all bytes equal would imply result true */
                     __CPROVER_forall {
                         size_t i; (i < str->len) ==> (str->bytes[i] == buf->buffer[i])
                     }));
        }
    }

    /* Unchanged fields: string */
    if (str != NULL) {
        assert(str->allocator == NULL);
        assert(str->len == old_str_bytes.size);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* Unchanged fields: byte buffer */
    if (buf != NULL) {
        assert(buf->allocator == tmp_buf.allocator);
        assert(buf->capacity == tmp_buf.capacity);
        assert(buf->buffer == tmp_buf.buffer);
        assert(buf->len == tmp_buf.len);
        assert_byte_from_buffer_matches(buf->buffer, &old_buf_bytes);
    }

    /* Validity invariants must still hold */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(buf));
}
