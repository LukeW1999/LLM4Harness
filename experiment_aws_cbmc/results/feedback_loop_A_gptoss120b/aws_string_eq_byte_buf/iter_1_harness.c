#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare nondeterministic inputs (may be NULL) */
    struct aws_string str_obj;
    struct aws_string *str = nondet_bool() ? &str_obj : NULL;

    struct aws_byte_buf buf_obj;
    struct aws_byte_buf *buf = nondet_bool() ? &buf_obj : NULL;

    /* 2. Set up preconditions for non‑NULL inputs */
    if (str) {
        /* keep things simple: zero length so no need to allocate flexible array */
        __CPROVER_assume(str->len == 0);
        /* allocator can be any pointer */
        str->allocator = (struct aws_allocator *)nondet_uintptr();
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (buf) {
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        /* also keep length zero to avoid needing to initialise buffer contents */
        __CPROVER_assume(buf->len == 0);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* 3. Save old state for immutability checks */
    struct aws_string old_str = {0};
    if (str) {
        old_str = *str;
    }

    struct aws_byte_buf old_buf = {0};
    if (buf) {
        old_buf = *buf;
    }

    /* 4. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 5. Post‑condition on return value */
    if (str == NULL && buf == NULL) {
        assert(result);
    } else if (str == NULL || buf == NULL) {
        assert(!result);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* 6. Unchanged fields for str (if non‑NULL) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes content unchanged – length is zero, nothing to compare */
    }

    /* 7. Unchanged fields for buf (if non‑NULL) */
    if (buf) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        /* buffer content unchanged – length is zero, nothing to compare */
    }

    /* 8. Validity invariants must still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
