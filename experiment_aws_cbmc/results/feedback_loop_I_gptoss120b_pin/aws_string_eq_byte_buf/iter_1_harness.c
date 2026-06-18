#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_string *str;
    if (nondet_bool()) {
        /* Allocate a non‑null string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        /* Allocate space for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        if (str) {
            str->allocator = aws_default_allocator();
            str->len = len;
            /* The bytes are part of the allocation; no further init needed */
            __CPROVER_assume(aws_string_is_valid(str));
        }
    } else {
        /* NULL string */
        str = NULL;
    }

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    buf.allocator = aws_default_allocator();
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    /* No additional validity predicate for aws_byte_buf, the boundedness is sufficient */

    /* 2. Save old state BEFORE calling */
    struct aws_string old_str_struct;
    struct aws_string *old_str_ptr = str;
    if (str) {
        old_str_struct = *str;
    }
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function never modifies its inputs, so the same assertions apply regardless of result */
    if (str == NULL) {
        /* When str is NULL the function must return false because buf is non‑NULL */
        assert(result == false);
    } else {
        /* When str is non‑NULL the result must equal the array comparison */
        bool expected = aws_array_eq(str->bytes, str->len, buf.buffer, buf.len);
        assert(result == expected);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    if (str) {
        assert(str->allocator == old_str_struct.allocator);
        assert(str->len == old_str_struct.len);
        /* The flexible array contents are not required to be unchanged for this function,
           but the memory region is not written to, so we do not assert on its contents. */
    } else {
        assert(str == old_str_ptr);
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);

    /* 6. Assert validity invariant always holds */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
