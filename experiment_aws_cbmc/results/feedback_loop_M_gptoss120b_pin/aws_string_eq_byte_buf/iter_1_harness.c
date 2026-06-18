#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare nondet pointers for inputs */
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = nondet_bool() ? NULL : &buf;

    /* 2. Initialize and bound the string if non‑NULL */
    if (str != NULL) {
        /* allocator is never used for mutation, set to default */
        str->allocator = aws_default_allocator();

        /* nondet length bounded by MAX_BUFFER_SIZE */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

        /* bytes are part of the allocation; no further init needed */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 3. Initialize and bound the byte buffer if non‑NULL */
    if (buf_ptr != NULL) {
        buf.allocator = aws_default_allocator();

        /* ensure the buffer member is allocated */
        ensure_byte_buf_has_allocated_buffer_member(&buf);

        /* bound capacity and length */
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        __CPROVER_assume(buf.len <= buf.capacity);
    }

    /* 4. Save old immutable state */
    struct aws_string *old_str = str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf_ptr != NULL) {
        save_byte_from_array(buf.buffer, buf.len, &old_buf_bytes);
    }

    /* 5. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* 6. Post‑condition: result matches specification */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
    }
    assert(result == expected);

    /* 7. Unchanged fields for non‑NULL inputs */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (buf_ptr != NULL) {
        assert(buf.allocator == old_buf.allocator);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.len == old_buf.len);
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_bytes);
    }

    /* 8. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
    }
}
