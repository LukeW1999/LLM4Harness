#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and nondeterministically initialize inputs */
    struct aws_string *str;
    struct aws_byte_buf buf_obj;
    const struct aws_byte_buf *buf;

    /* str may be NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate enough memory for the flexible array member */
        str = malloc(sizeof(struct aws_string) + len - 1);
        __CPROVER_assume(str != NULL);

        str->allocator = aws_default_allocator();
        str->len = len;
        /* bytes are part of the allocated block; they are left nondeterministic */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* buf may be NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        ensure_byte_buf_has_allocated_buffer_member(&buf_obj);
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_obj, MAX_BUFFER_SIZE));
        buf = &buf_obj;
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = str;
    struct aws_byte_buf old_buf = buf_obj; /* copy of whole struct */

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Post‑condition on return value */
    if (str == NULL && buf == NULL) {
        assert(result);
    } else if (str == NULL || buf == NULL) {
        assert(!result);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf_obj.buffer, buf_obj.len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert(str->bytes == old_str->bytes);
    }

    if (buf != NULL) {
        assert(buf_obj.capacity == old_buf.capacity);
        assert(buf_obj.len == old_buf.len);
        assert(buf_obj.buffer == old_buf.buffer);
        assert(buf_obj.allocator == old_buf.allocator);
    }

    /* 6. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(&buf_obj, MAX_BUFFER_SIZE));
    }
}
