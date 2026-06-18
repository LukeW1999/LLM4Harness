#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare nondet pointers for inputs */
    struct aws_string *str = nondet_bool() ? NULL : &(struct aws_string){0};
    struct aws_byte_buf *buf_ptr = nondet_bool() ? NULL : &(struct aws_byte_buf){0};

    /* 2. Initialize and bound the string if non‑NULL */
    if (str != NULL) {
        str->allocator = aws_default_allocator();

        /* nondet length bounded by MAX_BUFFER_SIZE */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

        /* allocate bytes for the string */
        str->bytes = malloc(str->len);
        __CPROVER_assume(str->bytes != NULL);

        /* ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 3. Initialize and bound the byte buffer if non‑NULL */
    if (buf_ptr != NULL) {
        buf_ptr->allocator = aws_default_allocator();

        /* ensure the buffer member is allocated */
        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);

        /* bound capacity and length */
        __CPROVER_assume(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
        __CPROVER_assume(buf_ptr->len <= buf_ptr->capacity);
    }

    /* 4. Save old immutable state */
    struct aws_string *old_str = str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = {0};
    struct store_byte_from_buffer old_buf_bytes;
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        save_byte_from_array(buf_ptr->buffer, buf_ptr->len, &old_buf_bytes);
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
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->len == old_buf.len);
        assert_byte_from_buffer_matches(buf_ptr->buffer, &old_buf_bytes);
    }

    /* 8. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
    }
}
